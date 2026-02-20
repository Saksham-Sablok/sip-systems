#ifndef ENUMS_H
#define ENUMS_H

#include <string>

namespace sip {

// SIP Frequency - how often the SIP executes
enum class SIPFrequency {
    WEEKLY,
    MONTHLY,
    QUARTERLY
};

// SIP State - lifecycle states of an SIP
enum class SIPState {
    ACTIVE,
    PAUSED,
    STOPPED
};

// Payment Status - status of a payment transaction
enum class PaymentStatus {
    PENDING,
    SUCCESS,
    FAILURE
};

// Transaction Type - type of transaction
enum class TransactionType {
    INSTALLMENT,
    LUMP_SUM
};

// Risk Level for mutual funds
enum class RiskLevel {
    LOW,
    MEDIUM,
    HIGH
};

// Fund Category
enum class FundCategory {
    EQUITY,
    DEBT,
    HYBRID,
    ELSS
};

// Helper functions for string conversion
inline std::string toString(SIPFrequency freq) {
    switch (freq) {
        case SIPFrequency::WEEKLY: return "WEEKLY";
        case SIPFrequency::MONTHLY: return "MONTHLY";
        case SIPFrequency::QUARTERLY: return "QUARTERLY";
        default: return "UNKNOWN";
    }
}

inline std::string toString(SIPState state) {
    switch (state) {
        case SIPState::ACTIVE: return "ACTIVE";
        case SIPState::PAUSED: return "PAUSED";
        case SIPState::STOPPED: return "STOPPED";
        default: return "UNKNOWN";
    }
}

inline std::string toString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::PENDING: return "PENDING";
        case PaymentStatus::SUCCESS: return "SUCCESS";
        case PaymentStatus::FAILURE: return "FAILURE";
        default: return "UNKNOWN";
    }
}

inline std::string toString(TransactionType type) {
    switch (type) {
        case TransactionType::INSTALLMENT: return "INSTALLMENT";
        case TransactionType::LUMP_SUM: return "LUMP_SUM";
        default: return "UNKNOWN";
    }
}

inline std::string toString(RiskLevel level) {
    switch (level) {
        case RiskLevel::LOW: return "LOW";
        case RiskLevel::MEDIUM: return "MEDIUM";
        case RiskLevel::HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

inline std::string toString(FundCategory category) {
    switch (category) {
        case FundCategory::EQUITY: return "EQUITY";
        case FundCategory::DEBT: return "DEBT";
        case FundCategory::HYBRID: return "HYBRID";
        case FundCategory::ELSS: return "ELSS";
        default: return "UNKNOWN";
    }
}

} // namespace sip

#endif // ENUMS_H
