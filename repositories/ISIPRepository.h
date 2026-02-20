#ifndef ISIP_REPOSITORY_H
#define ISIP_REPOSITORY_H

#include "IRepository.h"
#include "../models/SIP.h"
#include "../models/Enums.h"
#include <vector>

namespace sip {

/**
 * Repository interface for SIP entities.
 * Extends IRepository with SIP-specific query methods.
 */
class ISIPRepository : public IRepository<SIP> {
public:
    virtual ~ISIPRepository() = default;

    // Get all SIPs for a specific user
    virtual std::vector<SIP> getByUserId(const std::string& userId) const = 0;

    // Get all SIPs for a specific fund
    virtual std::vector<SIP> getByFundId(const std::string& fundId) const = 0;

    // Get all SIPs with a specific state
    virtual std::vector<SIP> getByState(SIPState state) const = 0;

    // Get all SIPs for a user with a specific state
    virtual std::vector<SIP> getByUserIdAndState(const std::string& userId, SIPState state) const = 0;

    // Get all active SIPs that are due for execution on a given date
    virtual std::vector<SIP> getDueSIPs(Date asOfDate) const = 0;
};

} // namespace sip

#endif // ISIP_REPOSITORY_H
