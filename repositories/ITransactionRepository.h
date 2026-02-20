#ifndef ITRANSACTION_REPOSITORY_H
#define ITRANSACTION_REPOSITORY_H

#include "IRepository.h"
#include "../models/Transaction.h"
#include "../models/Enums.h"
#include <vector>

namespace sip {

/**
 * Repository interface for Transaction entities.
 * Extends IRepository with transaction-specific query methods.
 */
class ITransactionRepository : public IRepository<Transaction> {
public:
    virtual ~ITransactionRepository() = default;

    // Get all transactions for a specific SIP
    virtual std::vector<Transaction> getBySipId(const std::string& sipId) const = 0;

    // Get transactions by status
    virtual std::vector<Transaction> getByStatus(PaymentStatus status) const = 0;

    // Get successful transactions for a SIP (for calculating totals)
    virtual std::vector<Transaction> getSuccessfulBySipId(const std::string& sipId) const = 0;
};

} // namespace sip

#endif // ITRANSACTION_REPOSITORY_H
