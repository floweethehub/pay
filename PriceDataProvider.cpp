/*
 * This file is part of the Flowee project
 * Copyright (C) 2021 Tom Zander <tom@flowee.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "PriceDataProvider.h"
#include "Logger.h"

#include <QLocale>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringBuilder>

// CoinGecko
static const char *CoinGeckoURL = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin-cash&vs_currencies=%1";
static const char *CoinGeckoJSONRoot = "bitcoin-cash";
// CoinGecko end

PriceDataProvider::PriceDataProvider(QObject *parent) : QObject(parent)
{
    QLocale here(QLocale::system());
    m_currency = here.currencySymbol(QLocale::CurrencyIsoCode);
    m_currencySymbolPrefix = here.currencySymbol(QLocale::CurrencySymbol);
    if (m_currency == QLatin1String("AZN")
            || m_currency == QLatin1String("ILS")
            || m_currency == QLatin1String("IRR")
            || m_currency == QLatin1String("KHR")
            || m_currency == QLatin1String("KZT")
            || m_currency == QLatin1String("PLN")
            || m_currency == QLatin1String("PYG")
            || m_currency == QLatin1String("RUB")
            || m_currency == QLatin1String("VND")) {
        // these currencies format the symbol after the numbers part.
        std::swap(m_currencySymbolPost, m_currencySymbolPrefix);
    }
    // drop the '.00' behind the prices as this country doesn't traditionlly do that
    m_dispayCents = !(m_currency == QLatin1String("JPY")
                   || m_currency == QLatin1String("JPY"));
    m_timer.setInterval(5 * 60 * 1000);
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(fetch()));
}

void PriceDataProvider::start()
{
    m_timer.start();
    fetch();
}

QString PriceDataProvider::formattedPrice(double amountSats, int price) const
{
    if (price == 0)
        return QString();
    qint64 fiatValue = amountSats * price;
    fiatValue = (fiatValue + 50000000) / qint64(100000000);

    // convert cheaply (low number of mallocs) to a price.
    // since our fiat is in cents, we assume we may add up to two leading zeros.
    static const QString priceTemplate("00%1");
    QString centsPrice = priceTemplate.arg(std::abs(fiatValue));
    const int offset = std::min(2, centsPrice.length() - 3); // number of those zeros to cut off again.

    if (m_dispayCents) {
        return m_currencySymbolPrefix
                % (fiatValue < 0 ? QLatin1String("-") : QLatin1String(""))
                % centsPrice.mid(offset, centsPrice.length() - 2 - offset)
                % QLocale::system().decimalPoint()
                % centsPrice.right(2)
                % m_currencySymbolPost;
    }
    else {
        return m_currencySymbolPrefix
                % (fiatValue < 0 ? QLatin1String("-") : QLatin1String(""))
                % centsPrice.mid(offset, centsPrice.length() - 2 - offset)
                % m_currencySymbolPost;
    }
}

void PriceDataProvider::fetch()
{
    QString url(CoinGeckoURL);
    QNetworkRequest req(QUrl(url.arg(m_currency.toLower())));
    m_reply = m_network.get(req);
    connect(m_reply, SIGNAL(finished()), this, SLOT(finishedDownload()));
}

void PriceDataProvider::finishedDownload()
{
    assert(m_reply);
    auto data = m_reply->readAll();
    m_reply->deleteLater();
    m_reply = nullptr;
    try {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isEmpty())
            throw std::runtime_error("Failed parsing the data");

        auto root = doc.object();
        // coingecko
        auto section = root.value(CoinGeckoJSONRoot).toObject();
        auto price = section.value(m_currency.toLower());
        if (price.isUndefined()) { // our provider does not support this coin.
            if (m_currency != "USD") {
                m_currency = "USD";
                m_currencySymbolPost.clear();
                m_currencySymbolPrefix = "$";
                fetch();
            }
            return;
        }
        m_currentPrice.price = price.toDouble() * 100;
        m_currentPrice.timestamp = time(nullptr);
        emit priceChanged();
    } catch (const std::runtime_error &error) {
        qWarning() << error.what();
        qWarning() << QString::fromUtf8(data);
    }
    logInfo() << "Current fiat price: " << m_currencySymbolPrefix << m_currentPrice.price << m_currencySymbolPost;
}
