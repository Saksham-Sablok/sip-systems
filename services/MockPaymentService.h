#ifndef MOCK_PAYMENT_SERVICE_H
#define MOCK_PAYMENT_SERVICE_H

#include "IPaymentService.h"
#include <unordered_map>
#include <random>

namespace sip {

/**
 * Mock implementation of IPaymentService.
 * Simulates payment processing with configurable success rate.
 */
class MockPaymentService : public IPaymentService {
private:
    PaymentCallback completionHandler;
    std::unordered_map<std::string, bool> processedCallbacks;  // For idempotency
    double successRate;  // Probability of payment success (0.0 to 1.0)
    bool autoComplete;   // If true, immediately calls callback; if false, waits for manual trigger
    
    // Pending payments waiting for manual completion
    std::unordered_map<std::string, std::pair<double, PaymentCallback>> pendingPayments;

public:
    /**
     * Constructor.
     * @param successRate Probability of success (default 1.0 = always succeed)
     * @param autoComplete If true, immediately processes payments
     */
    MockPaymentService(double successRate = 1.0, bool autoComplete = true)
        : successRate(successRate), autoComplete(autoComplete) {}

    void initiatePayment(const std::string& transactionId, 
                          double amount,
                          PaymentCallback callback) override {
        if (autoComplete) {
            // Simulate immediate payment completion
            PaymentStatus status = simulatePaymentResult();
            callback(transactionId, status);
            
            // Also call the global completion handler if set
            if (completionHandler) {
                completionHandler(transactionId, status);
            }
        } else {
            // Store for later manual completion
            pendingPayments[transactionId] = {amount, callback};
        }
    }

    void onPaymentCallback(const std::string& transactionId, 
                            PaymentStatus status) override {
        // Check idempotency
        if (processedCallbacks.find(transactionId) != processedCallbacks.end()) {
            return;  // Already processed
        }
        processedCallbacks[transactionId] = true;

        // Call completion handler
        if (completionHandler) {
            completionHandler(transactionId, status);
        }
    }

    void setPaymentCompletionHandler(PaymentCallback handler) override {
        completionHandler = handler;
    }

    // Test helper methods

    /**
     * Manually complete a pending payment (for testing).
     */
    void completePayment(const std::string& transactionId, PaymentStatus status) {
        auto it = pendingPayments.find(transactionId);
        if (it != pendingPayments.end()) {
            auto callback = it->second.second;
            pendingPayments.erase(it);
            callback(transactionId, status);
            
            if (completionHandler) {
                completionHandler(transactionId, status);
            }
        }
    }

    /**
     * Complete all pending payments with the given status.
     */
    void completeAllPending(PaymentStatus status) {
        auto pending = pendingPayments;  // Copy since we modify in loop
        for (const auto& pair : pending) {
            completePayment(pair.first, status);
        }
    }

    /**
     * Get number of pending payments.
     */
    size_t getPendingCount() const {
        return pendingPayments.size();
    }

    /**
     * Set success rate for auto-complete mode.
     */
    void setSuccessRate(double rate) {
        successRate = std::max(0.0, std::min(1.0, rate));
    }

    /**
     * Check if a callback was already processed.
     */
    bool isCallbackProcessed(const std::string& transactionId) const {
        return processedCallbacks.find(transactionId) != processedCallbacks.end();
    }

private:
    PaymentStatus simulatePaymentResult() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        
        return (dis(gen) < successRate) ? PaymentStatus::SUCCESS : PaymentStatus::FAILURE;
    }
};

} // namespace sip

#endif // MOCK_PAYMENT_SERVICE_H
