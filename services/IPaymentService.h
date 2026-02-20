#ifndef IPAYMENT_SERVICE_H
#define IPAYMENT_SERVICE_H

#include "../models/Enums.h"
#include <string>
#include <functional>

namespace sip {

/**
 * Payment callback function type.
 * Called when payment completes (success or failure).
 */
using PaymentCallback = std::function<void(const std::string& transactionId, PaymentStatus status)>;

/**
 * Service interface for payment operations.
 * This is a mock interface - actual payment is handled by external service.
 */
class IPaymentService {
public:
    virtual ~IPaymentService() = default;

    /**
     * Initiate a payment for the given amount.
     * 
     * @param transactionId Unique transaction identifier
     * @param amount Payment amount
     * @param callback Function to call when payment completes
     */
    virtual void initiatePayment(const std::string& transactionId, 
                                  double amount,
                                  PaymentCallback callback) = 0;

    /**
     * Process payment callback (for handling external callbacks).
     * 
     * @param transactionId Transaction identifier
     * @param status Payment status from external service
     */
    virtual void onPaymentCallback(const std::string& transactionId, 
                                    PaymentStatus status) = 0;

    /**
     * Set the callback handler for payment completions.
     */
    virtual void setPaymentCompletionHandler(PaymentCallback handler) = 0;
};

} // namespace sip

#endif // IPAYMENT_SERVICE_H
