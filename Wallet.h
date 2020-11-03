/*
 * This file is part of the Flowee project
 * Copyright (C) 2020 Tom Zander <tomz@freedommail.ch>
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
#ifndef FLOWEE_WALLET_H
#define FLOWEE_WALLET_H

#include <DataListenerInterface.h>
#include <PrivacySegment.h>

#include <primitives/key.h>
#include <primitives/pubkey.h>
#include <primitives/FastTransaction.h>

#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>

#include <QMutex>
#include <QString>
#include <QObject>

class WalletUnspentOutputsModel;

class Wallet : public QObject, public DataListenerInterface
{
    Q_OBJECT
public:
    /**
     * Create an empty, new, wallet.
     */
    static Wallet *createWallet(const boost::filesystem::path &basedir, uint16_t segmentId, const QString &name = QString());

    /**
     * Load existing wallet.
     * This throws should there not be any data found.
     */
    Wallet(const boost::filesystem::path &basedir, uint16_t segmentId);
    ~Wallet();

    class OutputRef {
    public:
        explicit OutputRef(int txIndex = 0, int outputIndex = 0); // invalid
        explicit OutputRef(uint64_t encoded);
        OutputRef(const OutputRef &output) = default;
        uint64_t encoded() const;

        inline int txIndex() const {
            return m_txid;
        }
        inline int outputIndex() const {
            return m_outputIndex;
        }

        inline void setTxIndex(int index) {
            m_txid = index;
        }
        inline void setOutputIndex(int index) {
            m_outputIndex = index;
        }

    private:
        uint32_t m_txid = 0; // index in m_walletTransactions
        uint16_t m_outputIndex = 0;
    };

    /**
     * @brief newTransactions announces a list of transactions pushed to us from a peer.
     * @param header the block header these transactions appeared in.
     * @param blockHeight the blockheight we know the header under.
     * @param blockTransactions The actual transactions.
     */
    void newTransactions(const BlockHeader &header, int blockHeight, const std::deque<Tx> &blockTransactions) override;
    // notify about unconfirmed Tx.
    void newTransaction(const Tx &tx) override;
    void setLastSynchedBlockHeight(int height) override;

    PrivacySegment *segment() const;

    void createNewPrivateKey(int currentBlockheight);
    bool addPrivateKey(const QString &privKey, int startBlockHeight);
    void saveWallet();

    /// return the current balance
    qint64 balance() const;

    // ^ TODO split into confirmed and unconfirmend

    /// return the amount of UTXOs that hold money
    int unspentOutputCount() const;
    /// return the amount of UTXOs ever created for this account.
    int historicalOutputCount() const;

    QString name() const;
    void setName(const QString &name);

    const uint256 &txid(OutputRef ref) const;
    Tx::Output txOutout(OutputRef ref) const;
    const CKey &unlockKey(OutputRef ref) const;

    CKeyID nextChangeAddress();

    struct OutputSet {
        std::vector<OutputRef> outputs;
        qint64 totalSats = 0;
        int fee = 0;
    };
    /**
     * @brief findInputsFor UTXO fulfilment algo finding the inputs for your tx.
     * @param output The amount of satoshis you want to make available (after fee).
     *          A special value of -1 indicates all outputs.
     * @param feePerByte fee per byte
     * @param txSize the size of the transaction before we add inputs
     * @param[out] change the amount of satoshis we over-provided for the expected \a output.
     * @return The references to the outputs we suggest you fund your tx with.
     */
    OutputSet findInputsFor(qint64 output, int feePerByte, int txSize, int64_t &change) const;

    bool isSingleAddressWallet() const;
    void setSingleAddressWallet(bool isSingleAddressWallet);

signals:
    void utxosChanged();
    void appendedTransactions(int firstNew, int count);
    void lastBlockSynchedChanged();

protected:
    Wallet();

private:
    void loadWallet();
    void loadSecrets();
    void saveSecrets();

    int findSecretFor(const Streaming::ConstBuffer &outputScript);

    void rebuildBloom();

    std::unique_ptr<PrivacySegment> m_segment;
    mutable QMutex m_lock;
    bool m_walletChanged = false;
    bool m_secretsChanged = false;

    struct WalletSecret {
        CKey privKey;
        CKeyID address;
        int initialHeight = -1;
    };

    int m_nextWalletSecretId = 1;
    int m_nextWalletTransactionId = 1;
    short m_bloomScore = 0;
    std::map<int, WalletSecret> m_walletSecrets;

    QString m_name;

    struct Output {
        int walletSecretId = -1;
        uint64_t value;
    };

    struct WalletTransaction {
        uint256 txid;
        uint256 minedBlock;
        int minedBlockHeight = 0;

        // One entry for inputs that spent outputs in this wallet.
        // The key is the input. They value is a composition of the output-index (lower 2 bytes)
        // and the int-key in m_walletTransactions is the middle 4 bytes.
        std::map<int, uint64_t> inputToWTX;
        std::map<int, Output> outputs; // output-index to Output (only ones we can/could spend)
    };

    WalletTransaction createWalletTransactionFromTx(const Tx &tx, const uint256 &txid);
    void saveTransaction(const Tx &tx);

    std::map<int, WalletTransaction> m_walletTransactions;

    typedef boost::unordered_map<uint256, int, HashShortener> TxIdCash;
    TxIdCash m_txidCash; // txid -> m_walletTransactions-id

    // cache
    std::map<uint64_t, uint64_t> m_unspentOutputs; // composited output -> value (in sat).
    // composited outputs that have been used in a transaction and should not be spent again.
    // the 'value' is the WalletTransaction index that actually was responsible for locking it.
    std::map<uint64_t, int> m_autoLockedOutputs;
    boost::filesystem::path m_basedir;

    friend class WalletHistoryModel;

    bool m_singleAddressWallet = false;
};

#endif
