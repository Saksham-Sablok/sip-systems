#ifndef IMARKET_PRICE_SERVICE_H
#define IMARKET_PRICE_SERVICE_H

#include <string>

namespace sip {

/**
 * Service interface for market price (NAV) operations.
 * In production, this would fetch real-time NAV from external sources.
 */
class IMarketPriceService {
public:
    virtual ~IMarketPriceService() = default;

    /**
     * Get the current NAV (Net Asset Value) for a fund.
     * 
     * @param fundId The fund identifier
     * @return Current NAV for the fund
     * @throws FundNotFoundException if fund doesn't exist
     */
    virtual double getCurrentNAV(const std::string& fundId) const = 0;

    /**
     * Update the NAV for a fund (for mock/testing purposes).
     * 
     * @param fundId The fund identifier
     * @param nav New NAV value
     */
    virtual void updateNAV(const std::string& fundId, double nav) = 0;
};

} // namespace sip

#endif // IMARKET_PRICE_SERVICE_H
