#ifndef INMEMORY_MUTUAL_FUND_REPOSITORY_H
#define INMEMORY_MUTUAL_FUND_REPOSITORY_H

#include "IMutualFundRepository.h"
#include <unordered_map>
#include <algorithm>

namespace sip {

/**
 * In-memory implementation of IMutualFundRepository.
 * Uses unordered_map for O(1) lookups by ID.
 */
class InMemoryMutualFundRepository : public IMutualFundRepository {
private:
    std::unordered_map<std::string, MutualFund> storage;

public:
    void add(const MutualFund& fund) override {
        storage[fund.getId()] = fund;
    }

    std::shared_ptr<MutualFund> getById(const std::string& id) const override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            return std::make_shared<MutualFund>(it->second);
        }
        return nullptr;
    }

    std::vector<MutualFund> getAll() const override {
        std::vector<MutualFund> result;
        result.reserve(storage.size());
        for (const auto& pair : storage) {
            result.push_back(pair.second);
        }
        return result;
    }

    bool update(const MutualFund& fund) override {
        auto it = storage.find(fund.getId());
        if (it != storage.end()) {
            it->second = fund;
            return true;
        }
        return false;
    }

    bool remove(const std::string& id) override {
        return storage.erase(id) > 0;
    }

    bool exists(const std::string& id) const override {
        return storage.find(id) != storage.end();
    }

    size_t count() const override {
        return storage.size();
    }

    std::vector<MutualFund> getByCategory(FundCategory category) const override {
        std::vector<MutualFund> result;
        for (const auto& pair : storage) {
            if (pair.second.getCategory() == category) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<MutualFund> getByRiskLevel(RiskLevel riskLevel) const override {
        std::vector<MutualFund> result;
        for (const auto& pair : storage) {
            if (pair.second.getRiskLevel() == riskLevel) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
};

} // namespace sip

#endif // INMEMORY_MUTUAL_FUND_REPOSITORY_H
