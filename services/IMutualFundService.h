#ifndef IMUTUAL_FUND_SERVICE_H
#define IMUTUAL_FUND_SERVICE_H

#include "../models/MutualFund.h"
#include "../models/Enums.h"
#include <vector>
#include <memory>

namespace sip {

/**
 * Service interface for mutual fund operations.
 */
class IMutualFundService {
public:
    virtual ~IMutualFundService() = default;

    // Get all available mutual funds
    virtual std::vector<MutualFund> getAllFunds() const = 0;

    // Get a specific fund by ID (throws if not found)
    virtual MutualFund getFundById(const std::string& fundId) const = 0;

    // Filter funds by category
    virtual std::vector<MutualFund> filterByCategory(FundCategory category) const = 0;

    // Filter funds by risk level
    virtual std::vector<MutualFund> filterByRiskLevel(RiskLevel riskLevel) const = 0;

    // Add a new fund (for seeding/admin purposes)
    virtual void addFund(const MutualFund& fund) = 0;

    // Check if a fund exists
    virtual bool fundExists(const std::string& fundId) const = 0;
};

} // namespace sip

#endif // IMUTUAL_FUND_SERVICE_H
