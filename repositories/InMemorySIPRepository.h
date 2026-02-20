#ifndef INMEMORY_SIP_REPOSITORY_H
#define INMEMORY_SIP_REPOSITORY_H

#include "ISIPRepository.h"
#include <unordered_map>
#include <set>

namespace sip {

/**
 * In-memory implementation of ISIPRepository.
 * Uses unordered_map for O(1) lookups by ID with secondary indexes.
 */
class InMemorySIPRepository : public ISIPRepository {
private:
    std::unordered_map<std::string, SIP> storage;
    std::unordered_map<std::string, std::set<std::string>> userIndex;   // userId -> set of sipIds
    std::unordered_map<std::string, std::set<std::string>> fundIndex;   // fundId -> set of sipIds

    void addToIndexes(const SIP& sip) {
        userIndex[sip.getUserId()].insert(sip.getId());
        fundIndex[sip.getFundId()].insert(sip.getId());
    }

    void removeFromIndexes(const SIP& sip) {
        userIndex[sip.getUserId()].erase(sip.getId());
        fundIndex[sip.getFundId()].erase(sip.getId());
    }

public:
    void add(const SIP& sip) override {
        storage[sip.getId()] = sip;
        addToIndexes(sip);
    }

    std::shared_ptr<SIP> getById(const std::string& id) const override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            return std::make_shared<SIP>(it->second);
        }
        return nullptr;
    }

    std::vector<SIP> getAll() const override {
        std::vector<SIP> result;
        result.reserve(storage.size());
        for (const auto& pair : storage) {
            result.push_back(pair.second);
        }
        return result;
    }

    bool update(const SIP& sip) override {
        auto it = storage.find(sip.getId());
        if (it != storage.end()) {
            // Remove from old indexes if userId/fundId changed
            removeFromIndexes(it->second);
            // Update
            it->second = sip;
            // Add to new indexes
            addToIndexes(sip);
            return true;
        }
        return false;
    }

    bool remove(const std::string& id) override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            removeFromIndexes(it->second);
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

    std::vector<SIP> getByUserId(const std::string& userId) const override {
        std::vector<SIP> result;
        auto it = userIndex.find(userId);
        if (it != userIndex.end()) {
            for (const auto& sipId : it->second) {
                auto sipIt = storage.find(sipId);
                if (sipIt != storage.end()) {
                    result.push_back(sipIt->second);
                }
            }
        }
        return result;
    }

    std::vector<SIP> getByFundId(const std::string& fundId) const override {
        std::vector<SIP> result;
        auto it = fundIndex.find(fundId);
        if (it != fundIndex.end()) {
            for (const auto& sipId : it->second) {
                auto sipIt = storage.find(sipId);
                if (sipIt != storage.end()) {
                    result.push_back(sipIt->second);
                }
            }
        }
        return result;
    }

    std::vector<SIP> getByState(SIPState state) const override {
        std::vector<SIP> result;
        for (const auto& pair : storage) {
            if (pair.second.getState() == state) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<SIP> getByUserIdAndState(const std::string& userId, SIPState state) const override {
        std::vector<SIP> result;
        auto userSips = getByUserId(userId);
        for (const auto& sip : userSips) {
            if (sip.getState() == state) {
                result.push_back(sip);
            }
        }
        return result;
    }

    std::vector<SIP> getDueSIPs(Date asOfDate) const override {
        std::vector<SIP> result;
        for (const auto& pair : storage) {
            const SIP& sip = pair.second;
            // Only ACTIVE SIPs can be due
            if (sip.getState() == SIPState::ACTIVE) {
                // Check if nextExecutionDate <= asOfDate
                if (sip.getNextExecutionDate() <= asOfDate) {
                    result.push_back(sip);
                }
            }
        }
        return result;
    }
};

} // namespace sip

#endif // INMEMORY_SIP_REPOSITORY_H
