#ifndef SIP_SERVICE_IMPL_H
#define SIP_SERVICE_IMPL_H

#include "ISIPService.h"
#include "IMutualFundService.h"
#include "../repositories/ISIPRepository.h"
#include "../repositories/IUserRepository.h"
#include "../utils/Exceptions.h"
#include "../utils/DateUtils.h"
#include "../utils/IdGenerator.h"
#include <memory>
#include <cmath>

namespace sip {

/**
 * Implementation of ISIPService.
 * Provides SIP management operations.
 */
class SIPServiceImpl : public ISIPService {
private:
    std::shared_ptr<ISIPRepository> sipRepository;
    std::shared_ptr<IUserRepository> userRepository;
    std::shared_ptr<IMutualFundService> fundService;

    void validateSIPExists(const std::string& sipId, SIP& outSip) const {
        auto sip = sipRepository->getById(sipId);
        if (!sip) {
            throw SIPNotFoundException(sipId);
        }
        outSip = *sip;
    }

public:
    SIPServiceImpl(std::shared_ptr<ISIPRepository> sipRepo,
                   std::shared_ptr<IUserRepository> userRepo,
                   std::shared_ptr<IMutualFundService> fundSvc)
        : sipRepository(std::move(sipRepo)),
          userRepository(std::move(userRepo)),
          fundService(std::move(fundSvc)) {}

    SIP createSIP(const std::string& userId, const std::string& fundId,
                  double amount, SIPFrequency frequency, Date startDate,
                  double stepUpPercentage = 0.0) override {
        // Validate user exists
        if (!userRepository->exists(userId)) {
            throw UserNotFoundException(userId);
        }
        
        // Validate fund exists
        if (!fundService->fundExists(fundId)) {
            throw FundNotFoundException(fundId);
        }
        
        // Validate amount
        if (amount <= 0) {
            throw ValidationException("SIP amount must be positive");
        }
        
        // Validate step-up percentage
        if (stepUpPercentage < 0) {
            throw ValidationException("Step-up percentage cannot be negative");
        }
        
        // Create SIP
        std::string sipId = IdGenerator::generateSipId();
        SIP sip(sipId, userId, fundId, amount, frequency, startDate, stepUpPercentage);
        
        sipRepository->add(sip);
        return sip;
    }

    void pauseSIP(const std::string& sipId) override {
        SIP sip;
        validateSIPExists(sipId, sip);
        
        if (sip.getState() == SIPState::PAUSED) {
            throw InvalidStateException(sipId, toString(sip.getState()), "pause");
        }
        if (sip.getState() == SIPState::STOPPED) {
            throw InvalidStateException(sipId, toString(sip.getState()), "pause");
        }
        
        sip.setState(SIPState::PAUSED);
        sipRepository->update(sip);
    }

    void unpauseSIP(const std::string& sipId) override {
        SIP sip;
        validateSIPExists(sipId, sip);
        
        if (sip.getState() == SIPState::ACTIVE) {
            throw InvalidStateException(sipId, toString(sip.getState()), "unpause");
        }
        if (sip.getState() == SIPState::STOPPED) {
            throw InvalidStateException(sipId, toString(sip.getState()), "unpause");
        }
        
        sip.setState(SIPState::ACTIVE);
        sipRepository->update(sip);
    }

    void stopSIP(const std::string& sipId) override {
        SIP sip;
        validateSIPExists(sipId, sip);
        
        if (sip.getState() == SIPState::STOPPED) {
            throw InvalidStateException(sipId, toString(sip.getState()), "stop");
        }
        
        sip.setState(SIPState::STOPPED);
        sipRepository->update(sip);
    }

    SIP getSIPById(const std::string& sipId) const override {
        auto sip = sipRepository->getById(sipId);
        if (!sip) {
            throw SIPNotFoundException(sipId);
        }
        return *sip;
    }

    std::vector<SIP> getSIPsByUser(const std::string& userId) const override {
        return sipRepository->getByUserId(userId);
    }

    std::vector<SIP> getSIPsByUserAndState(const std::string& userId, SIPState state) const override {
        return sipRepository->getByUserIdAndState(userId, state);
    }

    void modifyStepUp(const std::string& sipId, double newStepUpPercentage) override {
        SIP sip;
        validateSIPExists(sipId, sip);
        
        if (sip.getState() == SIPState::STOPPED) {
            throw InvalidStateException(sipId, toString(sip.getState()), "modify step-up");
        }
        
        if (newStepUpPercentage < 0) {
            throw ValidationException("Step-up percentage cannot be negative");
        }
        
        sip.setStepUpPercentage(newStepUpPercentage);
        sipRepository->update(sip);
    }

    double calculateCurrentInstallmentAmount(const std::string& sipId) const override {
        SIP sip = getSIPById(sipId);
        return calculateSteppedUpAmount(sip.getBaseAmount(), 
                                        sip.getStepUpPercentage(), 
                                        sip.getInstallmentCount() + 1);
    }

    void onPaymentSuccess(const std::string& sipId) override {
        SIP sip;
        validateSIPExists(sipId, sip);
        sip.incrementInstallmentCount();
        sipRepository->update(sip);
    }

    void updateNextExecutionDate(const std::string& sipId) override {
        SIP sip;
        validateSIPExists(sipId, sip);
        
        Date nextDate = calculateNextExecutionDate(sip.getNextExecutionDate(), sip.getFrequency());
        sip.setNextExecutionDate(nextDate);
        sipRepository->update(sip);
    }

private:
    /**
     * Calculate stepped-up amount using compound growth formula.
     * Formula: baseAmount * (1 + stepUpPercentage/100)^(installmentNumber - 1)
     */
    static double calculateSteppedUpAmount(double baseAmount, double stepUpPercentage, int installmentNumber) {
        if (stepUpPercentage <= 0 || installmentNumber <= 1) {
            return baseAmount;
        }
        double factor = std::pow(1.0 + stepUpPercentage / 100.0, installmentNumber - 1);
        return baseAmount * factor;
    }

    /**
     * Calculate the next execution date based on frequency.
     */
    static Date calculateNextExecutionDate(Date currentDate, SIPFrequency frequency) {
        switch (frequency) {
            case SIPFrequency::WEEKLY:
                return DateUtils::addWeeks(currentDate, 1);
            case SIPFrequency::MONTHLY:
                return DateUtils::addMonths(currentDate, 1);
            case SIPFrequency::QUARTERLY:
                return DateUtils::addQuarters(currentDate, 1);
            default:
                return DateUtils::addMonths(currentDate, 1);
        }
    }
};

} // namespace sip

#endif // SIP_SERVICE_IMPL_H
