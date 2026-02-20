#ifndef SIP_SCHEDULER_H
#define SIP_SCHEDULER_H

#include "../services/ISIPService.h"
#include "../services/IMarketPriceService.h"
#include "../services/IPaymentService.h"
#include "../repositories/ISIPRepository.h"
#include "../repositories/ITransactionRepository.h"
#include "../utils/DateUtils.h"
#include "../utils/IdGenerator.h"
#include "../utils/Exceptions.h"
#include <memory>
#include <iostream>
#include <cmath>

namespace sip {

/**
 * SIP Scheduler - Executes due SIPs based on their schedule.
 */
class SIPScheduler {
private:
    std::shared_ptr<ISIPRepository> sipRepository;
    std::shared_ptr<ITransactionRepository> transactionRepository;
    std::shared_ptr<IMarketPriceService> marketPriceService;
    std::shared_ptr<IPaymentService> paymentService;
    std::shared_ptr<ISIPService> sipService;

public:
    SIPScheduler(std::shared_ptr<ISIPRepository> sipRepo,
                 std::shared_ptr<ITransactionRepository> txnRepo,
                 std::shared_ptr<IMarketPriceService> marketSvc,
                 std::shared_ptr<IPaymentService> paymentSvc,
                 std::shared_ptr<ISIPService> sipSvc)
        : sipRepository(std::move(sipRepo)),
          transactionRepository(std::move(txnRepo)),
          marketPriceService(std::move(marketSvc)),
          paymentService(std::move(paymentSvc)),
          sipService(std::move(sipSvc)) {}

    /**
     * Check if an SIP is due for execution on the given date.
     */
    bool isDue(const SIP& sip, Date asOfDate) const {
        // Only ACTIVE SIPs can be due
        if (sip.getState() != SIPState::ACTIVE) {
            return false;
        }
        
        // Check if nextExecutionDate <= asOfDate
        return DateUtils::isOnOrBefore(sip.getNextExecutionDate(), asOfDate);
    }

    /**
     * Execute all SIPs that are due on the given date.
     * Returns the number of SIPs processed.
     */
    int executeDueSIPs(Date asOfDate) {
        std::vector<SIP> dueSIPs = sipRepository->getDueSIPs(asOfDate);
        int processedCount = 0;

        for (const auto& sip : dueSIPs) {
            try {
                executeSIP(sip, asOfDate);
                processedCount++;
            } catch (const std::exception& e) {
                // Log error but continue processing other SIPs
                std::cerr << "Error executing SIP " << sip.getId() << ": " << e.what() << std::endl;
            }
        }

        return processedCount;
    }

    /**
     * Execute a single SIP installment.
     */
    void executeSIP(const SIP& sip, Date executionDate) {
        // Skip if not ACTIVE
        if (sip.getState() != SIPState::ACTIVE) {
            return;
        }

        // Get current NAV
        double nav = marketPriceService->getCurrentNAV(sip.getFundId());
        
        // Calculate installment amount (with step-up)
        double amount = calculateSteppedUpAmount(sip.getBaseAmount(), 
                                                  sip.getStepUpPercentage(), 
                                                  sip.getInstallmentCount() + 1);
        
        // Calculate units
        double units = amount / nav;
        
        // Create transaction
        std::string txnId = IdGenerator::generateTransactionId();
        Transaction txn(txnId, sip.getId(), amount, nav, executionDate, TransactionType::INSTALLMENT);
        txn.setUnits(units);
        txn.setStatus(PaymentStatus::PENDING);
        transactionRepository->add(txn);
        
        // Initiate payment with callback
        std::string sipId = sip.getId();
        paymentService->initiatePayment(txnId, amount, 
            [this, txnId, sipId](const std::string& transactionId, PaymentStatus status) {
                this->handlePaymentCallback(transactionId, sipId, status);
            });
    }

private:
    /**
     * Handle payment callback.
     */
    void handlePaymentCallback(const std::string& transactionId, 
                                const std::string& sipId, 
                                PaymentStatus status) {
        auto txn = transactionRepository->getById(transactionId);
        if (!txn) {
            return;
        }
        
        // Idempotent check - skip if already processed
        if (txn->isCallbackProcessed()) {
            return;
        }
        
        // Update transaction
        txn->setStatus(status);
        txn->setCallbackProcessed(true);
        transactionRepository->update(*txn);
        
        if (status == PaymentStatus::SUCCESS) {
            // Increment installment count
            sipService->onPaymentSuccess(sipId);
            // Update next execution date
            sipService->updateNextExecutionDate(sipId);
        }
        // For FAILURE, we don't increment or update next execution date
    }

    /**
     * Calculate stepped-up amount.
     */
    static double calculateSteppedUpAmount(double baseAmount, double stepUpPercentage, int installmentNumber) {
        if (stepUpPercentage <= 0 || installmentNumber <= 1) {
            return baseAmount;
        }
        double factor = std::pow(1.0 + stepUpPercentage / 100.0, installmentNumber - 1);
        return baseAmount * factor;
    }
};

} // namespace sip

#endif // SIP_SCHEDULER_H
