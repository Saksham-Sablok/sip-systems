#ifndef IPORTFOLIO_SERVICE_H
#define IPORTFOLIO_SERVICE_H

#include "../models/SIP.h"
#include "../models/Transaction.h"
#include "../models/Enums.h"
#include <vector>
#include <string>

namespace sip {

/**
 * Portfolio summary data structure.
 */
struct PortfolioSummary {
    double totalInvested;
    double totalCurrentValue;
    double totalUnits;
    double gainLoss;
    double gainLossPercentage;
    int activeSIPCount;
    int pausedSIPCount;
    int stoppedSIPCount;

    PortfolioSummary() 
        : totalInvested(0), totalCurrentValue(0), totalUnits(0),
          gainLoss(0), gainLossPercentage(0), 
          activeSIPCount(0), pausedSIPCount(0), stoppedSIPCount(0) {}
};

/**
 * SIP portfolio details.
 */
struct SIPPortfolioItem {
    SIP sip;
    std::string fundName;
    double totalInvested;
    double totalUnits;
    double currentValue;
    double currentNav;
    double gainLoss;
    double gainLossPercentage;
    double currentInstallmentAmount;
    double nextInstallmentAmount;

    SIPPortfolioItem() 
        : totalInvested(0), totalUnits(0), currentValue(0), currentNav(0),
          gainLoss(0), gainLossPercentage(0), 
          currentInstallmentAmount(0), nextInstallmentAmount(0) {}
};

/**
 * Service interface for portfolio operations.
 */
class IPortfolioService {
public:
    virtual ~IPortfolioService() = default;

    // Get all SIPs for a user with portfolio details
    virtual std::vector<SIPPortfolioItem> getUserPortfolio(const std::string& userId) const = 0;

    // Get portfolio summary for a user
    virtual PortfolioSummary getPortfolioSummary(const std::string& userId) const = 0;

    // Filter user's SIPs by state
    virtual std::vector<SIPPortfolioItem> filterByState(const std::string& userId, SIPState state) const = 0;

    // Get transaction history for a specific SIP
    virtual std::vector<Transaction> getTransactionHistory(const std::string& sipId) const = 0;

    // Calculate total invested for an SIP
    virtual double calculateTotalInvested(const std::string& sipId) const = 0;

    // Calculate total units for an SIP
    virtual double calculateTotalUnits(const std::string& sipId) const = 0;

    // Calculate current value for an SIP
    virtual double calculateCurrentValue(const std::string& sipId) const = 0;
};

} // namespace sip

#endif // IPORTFOLIO_SERVICE_H
