#ifndef IMUTUAL_FUND_REPOSITORY_H
#define IMUTUAL_FUND_REPOSITORY_H

#include "IRepository.h"
#include "../models/MutualFund.h"
#include "../models/Enums.h"
#include <vector>

namespace sip {

/**
 * Repository interface for MutualFund entities.
 * Extends IRepository with fund-specific query methods.
 */
class IMutualFundRepository : public IRepository<MutualFund> {
public:
    virtual ~IMutualFundRepository() = default;

    // Filter by category (EQUITY, DEBT, HYBRID, ELSS)
    virtual std::vector<MutualFund> getByCategory(FundCategory category) const = 0;

    // Filter by risk level (LOW, MEDIUM, HIGH)
    virtual std::vector<MutualFund> getByRiskLevel(RiskLevel riskLevel) const = 0;
};

} // namespace sip

#endif // IMUTUAL_FUND_REPOSITORY_H
