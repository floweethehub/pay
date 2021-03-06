/*
 * This file is part of the Flowee project
 * Copyright (C) 2020-2021 Tom Zander <tom@flowee.org>
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
#include "BitcoinValue.h"
#include "FloweePay.h"

#include <QClipboard>
#include <QGuiApplication>

BitcoinValue::BitcoinValue(QObject *parent)
    : QObject(parent),
      m_value(0)
{
    connect (FloweePay::instance(), &FloweePay::unitChanged, this, [this]() {
        setStringValue(m_typedNumber);
    });
}

qint64 BitcoinValue::value() const
{
    return m_value;
}

void BitcoinValue::setValue(qint64 value)
{
    if (m_value == value)
        return;
    m_value = value;
    emit valueChanged();
}

void BitcoinValue::addNumber(QChar number)
{
    int pos = m_typedNumber.indexOf('.');
    if (pos > -1 && m_typedNumber.size() - pos - FloweePay::instance()->unitAllowedDecimals() > 0)
        return;
    m_typedNumber += number;
    while (((pos < 0 && m_typedNumber.size() > 1) || pos > 1) && m_typedNumber.startsWith('0'))
        m_typedNumber = m_typedNumber.mid(1);

    setStringValue(m_typedNumber);
    emit enteredStringChanged();
}

void BitcoinValue::addSeparator()
{
    if (m_typedNumber.indexOf('.') == -1) {
        m_typedNumber += '.';
        if (m_typedNumber.size() == 1)
            m_typedNumber = "0.";
        setStringValue(m_typedNumber);
        emit enteredStringChanged();
    }
}

void BitcoinValue::paste()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    assert(clipboard);
    QString originalText = clipboard->text().trimmed();
    bool started = false;
    for (int i = 0; i < originalText.size(); ++i) {
        auto k = originalText.at(i);
        if (k.isDigit()) {
            started = true;
            addNumber(k);
        }
        else if ((started || (originalText.size() > i + 1 && originalText.at(i+1).isDigit()))
                  && (k.unicode() == ',' || k.unicode() == '.')) {
            addSeparator();
        }
        else if (started)
            return;
    }
}

void BitcoinValue::backspacePressed()
{
    if (m_typedNumber.size() == 1)
        m_typedNumber = "0";
    else
        m_typedNumber = m_typedNumber.left(m_typedNumber.size() - 1);
    setStringValue(m_typedNumber);
    emit enteredStringChanged();
}

QString BitcoinValue::stringForValue() const
{
    return FloweePay::instance()->priceToString(m_value);
}

void BitcoinValue::setStringValue(const QString &value)
{
    int separator = value.indexOf('.');
    QString before, after;
    if (separator == -1) {
        before = value;
        after = "000000000";
    } else {
        before = value.left(separator);
        after = value.mid(separator + 1);
    }
    qint64 newVal = before.toLong();
    const int unitConfigDecimals = FloweePay::instance()->unitAllowedDecimals();
    for (int i = 0; i < unitConfigDecimals; ++i) {
        newVal *= 10;
    }
    while (after.size() < unitConfigDecimals)
        after += '0';

    newVal += after.toInt();
    setValue(newVal);
}

QString BitcoinValue::enteredString() const
{
    const char decimalPoint = QLocale::system().decimalPoint().unicode();
    if (decimalPoint != '.') { // make the user-visible one always use the locale-aware decimalpoint.
        QString answer(m_typedNumber);
        answer.replace('.', decimalPoint);
        return answer;
    }
    return m_typedNumber;
}

void BitcoinValue::setEnteredString(const QString &s)
{
    if (m_typedNumber == s)
        return;
    bool decimal = false;
    for (int i = 0; i < s.size(); ++i) {
        if (!s.at(i).isDigit()) {
            if (s.at(i) == '.') {
                if (decimal)
                    throw std::runtime_error("Multiple decimals");
                decimal = true;
            } else {
                if (decimal)
                    throw std::runtime_error("Contains illegal chars");
            }
        }
    }
    // I didn't check for length...
    m_typedNumber = s;
    emit enteredStringChanged();
}
