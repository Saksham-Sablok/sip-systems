#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <chrono>
#include "Enums.h"

namespace sip {

using Date = std::chrono::system_clock::time_point;

class Transaction {
private:
    std::string id;
    std::string sipId;
    double amount;
    double units;
    double nav;
    PaymentStatus status;
    Date date;
    TransactionType type;
    bool callbackProcessed;  // For idempotent callback processing

public:
    Transaction() : amount(0.0), units(0.0), nav(0.0), 
                    status(PaymentStatus::PENDING), 
                    type(TransactionType::INSTALLMENT),
                    callbackProcessed(false) {}
    
    Transaction(const std::string& id, const std::string& sipId, double amount,
                double nav, Date date, TransactionType type = TransactionType::INSTALLMENT)
        : id(id), sipId(sipId), amount(amount), units(0.0), nav(nav),
          status(PaymentStatus::PENDING), date(date), type(type), callbackProcessed(false) {
        // Calculate units: amount / nav
        if (nav > 0) {
            units = amount / nav;
        }
    }

    // Getters
    const std::string& getId() const { return id; }
    const std::string& getSipId() const { return sipId; }
    double getAmount() const { return amount; }
    double getUnits() const { return units; }
    double getNav() const { return nav; }
    PaymentStatus getStatus() const { return status; }
    Date getDate() const { return date; }
    TransactionType getType() const { return type; }
    bool isCallbackProcessed() const { return callbackProcessed; }

    // Setters
    void setId(const std::string& id) { this->id = id; }
    void setSipId(const std::string& sipId) { this->sipId = sipId; }
    void setAmount(double amount) { this->amount = amount; }
    void setUnits(double units) { this->units = units; }
    void setNav(double nav) { this->nav = nav; }
    void setStatus(PaymentStatus status) { this->status = status; }
    void setDate(Date date) { this->date = date; }
    void setType(TransactionType type) { this->type = type; }
    void setCallbackProcessed(bool processed) { this->callbackProcessed = processed; }

    // Recalculate units based on amount and NAV
    void calculateUnits() {
        if (nav > 0) {
            units = amount / nav;
        }
    }

    // Display helper
    std::string toString() const {
        return "Transaction{id=" + id + ", sipId=" + sipId +
               ", amount=" + std::to_string(amount) +
               ", units=" + std::to_string(units) +
               ", nav=" + std::to_string(nav) +
               ", status=" + sip::toString(status) +
               ", type=" + sip::toString(type) + "}";
    }
};

} // namespace sip

#endif // TRANSACTION_H
