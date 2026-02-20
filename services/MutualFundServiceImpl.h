#ifndef MUTUAL_FUND_SERVICE_IMPL_H
#define MUTUAL_FUND_SERVICE_IMPL_H

#include "IMutualFundService.h"
#include "../repositories/IMutualFundRepository.h"
#include "../utils/Exceptions.h"
#include <memory>

namespace sip {

/**
 * Implementation of IMutualFundService.
 * Provides mutual fund catalog operations.
 */
class MutualFundServiceImpl : public IMutualFundService {
private:
    std::shared_ptr<IMutualFundRepository> fundRepository;

public:
    explicit MutualFundServiceImpl(std::shared_ptr<IMutualFundRepository> fundRepo)
        : fundRepository(std::move(fundRepo)) {}

    std::vector<MutualFund> getAllFunds() const override {
        return fundRepository->getAll();
    }

    MutualFund getFundById(const std::string& fundId) const override {
        auto fund = fundRepository->getById(fundId);
        if (!fund) {
            throw FundNotFoundException(fundId);
        }
        return *fund;
    }

    std::vector<MutualFund> filterByCategory(FundCategory category) const override {
        return fundRepository->getByCategory(category);
    }

    std::vector<MutualFund> filterByRiskLevel(RiskLevel riskLevel) const override {
        return fundRepository->getByRiskLevel(riskLevel);
    }

    void addFund(const MutualFund& fund) override {
        if (fund.getId().empty()) {
            throw ValidationException("Fund ID cannot be empty");
        }
        if (fund.getName().empty()) {
            throw ValidationException("Fund name cannot be empty");
        }
        if (fund.getNav() <= 0) {
            throw ValidationException("Fund NAV must be positive");
        }
        fundRepository->add(fund);
    }

    bool fundExists(const std::string& fundId) const override {
        return fundRepository->exists(fundId);
    }
};

} // namespace sip

#endif // MUTUAL_FUND_SERVICE_IMPL_H
