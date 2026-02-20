#ifndef INMEMORY_TRANSACTION_REPOSITORY_H
#define INMEMORY_TRANSACTION_REPOSITORY_H

#include "ITransactionRepository.h"
#include <unordered_map>
#include <set>

namespace sip {

/**
 * In-memory implementation of ITransactionRepository.
 * Uses unordered_map for O(1) lookups by ID with sipId index.
 */
class InMemoryTransactionRepository : public ITransactionRepository {
private:
    std::unordered_map<std::string, Transaction> storage;
    std::unordered_map<std::string, std::set<std::string>> sipIndex;  // sipId -> set of transactionIds

public:
    void add(const Transaction& transaction) override {
        storage[transaction.getId()] = transaction;
        sipIndex[transaction.getSipId()].insert(transaction.getId());
    }

    std::shared_ptr<Transaction> getById(const std::string& id) const override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            return std::make_shared<Transaction>(it->second);
        }
        return nullptr;
    }

    std::vector<Transaction> getAll() const override {
        std::vector<Transaction> result;
        result.reserve(storage.size());
        for (const auto& pair : storage) {
            result.push_back(pair.second);
        }
        return result;
    }

    bool update(const Transaction& transaction) override {
        auto it = storage.find(transaction.getId());
        if (it != storage.end()) {
            // Update sipIndex if sipId changed
            if (it->second.getSipId() != transaction.getSipId()) {
                sipIndex[it->second.getSipId()].erase(transaction.getId());
                sipIndex[transaction.getSipId()].insert(transaction.getId());
            }
            it->second = transaction;
            return true;
        }
        return false;
    }

    bool remove(const std::string& id) override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            sipIndex[it->second.getSipId()].erase(id);
            storage.erase(it);
            return true;
        }
        return false;
    }

    bool exists(const std::string& id) const override {
        return storage.find(id) != storage.end();
    }

    size_t count() const override {
        return storage.size();
    }

    std::vector<Transaction> getBySipId(const std::string& sipId) const override {
        std::vector<Transaction> result;
        auto it = sipIndex.find(sipId);
        if (it != sipIndex.end()) {
            for (const auto& txnId : it->second) {
                auto txnIt = storage.find(txnId);
                if (txnIt != storage.end()) {
                    result.push_back(txnIt->second);
                }
            }
        }
        return result;
    }

    std::vector<Transaction> getByStatus(PaymentStatus status) const override {
        std::vector<Transaction> result;
        for (const auto& pair : storage) {
            if (pair.second.getStatus() == status) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<Transaction> getSuccessfulBySipId(const std::string& sipId) const override {
        std::vector<Transaction> result;
        auto transactions = getBySipId(sipId);
        for (const auto& txn : transactions) {
            if (txn.getStatus() == PaymentStatus::SUCCESS) {
                result.push_back(txn);
            }
        }
        return result;
    }
};

} // namespace sip

#endif // INMEMORY_TRANSACTION_REPOSITORY_H
