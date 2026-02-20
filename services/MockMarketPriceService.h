#ifndef MOCK_MARKET_PRICE_SERVICE_H
#define MOCK_MARKET_PRICE_SERVICE_H

#include "IMarketPriceService.h"
#include "../utils/Exceptions.h"
#include <unordered_map>
#include <random>

namespace sip {

/**
 * Mock implementation of IMarketPriceService.
 * Provides configurable NAV values for testing.
 */
class MockMarketPriceService : public IMarketPriceService {
private:
    std::unordered_map<std::string, double> navData;
    bool enablePriceFluctuation;
    double fluctuationRange;  // +/- percentage for price fluctuation

public:
    /**
     * Constructor.
     * @param enableFluctuation If true, prices will fluctuate slightly each call
     * @param range Fluctuation range as percentage (e.g., 0.02 = +/-2%)
     */
    MockMarketPriceService(bool enableFluctuation = false, double range = 0.02)
        : enablePriceFluctuation(enableFluctuation), fluctuationRange(range) {}

    double getCurrentNAV(const std::string& fundId) const override {
        auto it = navData.find(fundId);
        if (it == navData.end()) {
            throw FundNotFoundException(fundId);
        }
        
        double baseNav = it->second;
        
        if (enablePriceFluctuation) {
            // Apply random fluctuation
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(-fluctuationRange, fluctuationRange);
            baseNav *= (1.0 + dis(gen));
        }
        
        return baseNav;
    }

    void updateNAV(const std::string& fundId, double nav) override {
        if (nav <= 0) {
            throw ValidationException("NAV must be positive");
        }
        navData[fundId] = nav;
    }

    /**
     * Set NAV for multiple funds at once.
     */
    void setNAVs(const std::unordered_map<std::string, double>& navs) {
        for (const auto& pair : navs) {
            navData[pair.first] = pair.second;
        }
    }

    /**
     * Enable/disable price fluctuation.
     */
    void setFluctuationEnabled(bool enabled) {
        enablePriceFluctuation = enabled;
    }

    /**
     * Set fluctuation range.
     */
    void setFluctuationRange(double range) {
        fluctuationRange = range;
    }

    /**
     * Get the stored NAV (without fluctuation).
     */
    double getStoredNAV(const std::string& fundId) const {
        auto it = navData.find(fundId);
        if (it == navData.end()) {
            throw FundNotFoundException(fundId);
        }
        return it->second;
    }

    /**
     * Simulate market movement - increase/decrease all prices.
     * @param percentage Change as decimal (e.g., 0.05 = 5% increase, -0.03 = 3% decrease)
     */
    void simulateMarketMovement(double percentage) {
        for (auto& pair : navData) {
            pair.second *= (1.0 + percentage);
        }
    }
};

} // namespace sip

#endif // MOCK_MARKET_PRICE_SERVICE_H
