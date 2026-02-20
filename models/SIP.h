#ifndef SIP_H
#define SIP_H

#include <string>
#include <chrono>
#include "Enums.h"

namespace sip {

using Date = std::chrono::system_clock::time_point;

class SIP {
private:
    std::string id;
    std::string userId;
    std::string fundId;
    double baseAmount;
    SIPFrequency frequency;
    SIPState state;
    Date startDate;
    Date nextExecutionDate;
    int installmentCount;
    double stepUpPercentage;  // Percentage increase per installment (0 = no step-up)

public:
    SIP() : baseAmount(0.0), frequency(SIPFrequency::MONTHLY), 
            state(SIPState::ACTIVE), installmentCount(0), stepUpPercentage(0.0) {}
    
    SIP(const std::string& id, const std::string& userId, const std::string& fundId,
        double baseAmount, SIPFrequency frequency, Date startDate, double stepUpPercentage = 0.0)
        : id(id), userId(userId), fundId(fundId), baseAmount(baseAmount),
          frequency(frequency), state(SIPState::ACTIVE), startDate(startDate),
          nextExecutionDate(startDate), installmentCount(0), stepUpPercentage(stepUpPercentage) {}

    // Getters
    const std::string& getId() const { return id; }
    const std::string& getUserId() const { return userId; }
    const std::string& getFundId() const { return fundId; }
    double getBaseAmount() const { return baseAmount; }
    SIPFrequency getFrequency() const { return frequency; }
    SIPState getState() const { return state; }
    Date getStartDate() const { return startDate; }
    Date getNextExecutionDate() const { return nextExecutionDate; }
    int getInstallmentCount() const { return installmentCount; }
    double getStepUpPercentage() const { return stepUpPercentage; }

    // Setters
    void setId(const std::string& id) { this->id = id; }
    void setUserId(const std::string& userId) { this->userId = userId; }
    void setFundId(const std::string& fundId) { this->fundId = fundId; }
    void setBaseAmount(double baseAmount) { this->baseAmount = baseAmount; }
    void setFrequency(SIPFrequency frequency) { this->frequency = frequency; }
    void setState(SIPState state) { this->state = state; }
    void setStartDate(Date startDate) { this->startDate = startDate; }
    void setNextExecutionDate(Date nextExecutionDate) { this->nextExecutionDate = nextExecutionDate; }
    void setInstallmentCount(int count) { this->installmentCount = count; }
    void setStepUpPercentage(double percentage) { this->stepUpPercentage = percentage; }

    // Increment installment count
    void incrementInstallmentCount() { ++installmentCount; }

    // Display helper
    std::string toString() const {
        return "SIP{id=" + id + ", userId=" + userId + ", fundId=" + fundId +
               ", baseAmount=" + std::to_string(baseAmount) +
               ", frequency=" + sip::toString(frequency) +
               ", state=" + sip::toString(state) +
               ", installmentCount=" + std::to_string(installmentCount) +
               ", stepUpPercentage=" + std::to_string(stepUpPercentage) + "%}";
    }
};

} // namespace sip

#endif // SIP_H
