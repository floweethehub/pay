/*
 * This file is part of the Flowee project
 * Copyright (C) 2020 Tom Zander <tom@flowee.org>
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
#ifndef PORTFOLIODATAPROVIDER_H
#define PORTFOLIODATAPROVIDER_H

#include <QObject>
#include "AccountInfo.h"

class Wallet;
class BitcoinValue;
class Payment;

class PortfolioDataProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<AccountInfo*> accounts READ accounts NOTIFY accountsChanged)
    Q_PROPERTY(AccountInfo* current READ current WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(double totalBalance READ totalBalance NOTIFY totalBalanceChanged)
public:
    explicit PortfolioDataProvider(QObject *parent = nullptr);

    QList<AccountInfo*> accounts() const;

    AccountInfo *current() const;
    void setCurrent(AccountInfo *item);

    Q_INVOKABLE QObject *startPayToAddress(const QString &address, BitcoinValue *bitcoinValue);
    Q_INVOKABLE QObject *startPayAllToAddress(const QString &address);

    void selectDefaultWallet();

    double totalBalance() const;

public slots:
    void addWalletAccount(Wallet *wallet);

signals:
    void accountsChanged();
    void currentChanged();
    void totalBalanceChanged();

private slots:
    void walletChangedPriority();

private:
    Payment* createPaymentObject(const QString &address, double value);

    QList<Wallet*> m_accounts;
    QList<AccountInfo*> m_accountInfos;

    int m_currentAccount = -1;
};

#endif
