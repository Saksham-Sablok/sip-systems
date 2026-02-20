#ifndef ISIP_SERVICE_H
#define ISIP_SERVICE_H

#include "../models/SIP.h"
#include "../models/Enums.h"
#include <vector>
#include <memory>

namespace sip {

/**
 * Service interface for SIP management operations.
 */
class ISIPService {
public:
    virtual ~ISIPService() = default;

    // Create a new SIP
    virtual SIP createSIP(const std::string& userId, const std::string& fundId,
                          double amount, SIPFrequency frequency, Date startDate,
                          double stepUpPercentage = 0.0) = 0;

    // Pause an active SIP
    virtual void pauseSIP(const std::string& sipId) = 0;

    // Unpause a paused SIP
    virtual void unpauseSIP(const std::string& sipId) = 0;

    // Stop an SIP (terminal state)
    virtual void stopSIP(const std::string& sipId) = 0;

    // Get SIP by ID
    virtual SIP getSIPById(const std::string& sipId) const = 0;

    // Get all SIPs for a user
    virtual std::vector<SIP> getSIPsByUser(const std::string& userId) const = 0;

    // Get SIPs by user and state
    virtual std::vector<SIP> getSIPsByUserAndState(const std::string& userId, SIPState state) const = 0;

    // Modify step-up percentage
    virtual void modifyStepUp(const std::string& sipId, double newStepUpPercentage) = 0;

    // Calculate current installment amount (considering step-up)
    virtual double calculateCurrentInstallmentAmount(const std::string& sipId) const = 0;

    // Update SIP after successful payment
    virtual void onPaymentSuccess(const std::string& sipId) = 0;

    // Update next execution date
    virtual void updateNextExecutionDate(const std::string& sipId) = 0;
};

} // namespace sip

#endif // ISIP_SERVICE_H
