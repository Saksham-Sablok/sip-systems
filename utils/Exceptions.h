#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace sip {

/**
 * Base exception for SIP system errors.
 */
class SIPSystemException : public std::runtime_error {
public:
    explicit SIPSystemException(const std::string& message) 
        : std::runtime_error(message) {}
};

/**
 * Thrown when a mutual fund is not found.
 */
class FundNotFoundException : public SIPSystemException {
public:
    explicit FundNotFoundException(const std::string& fundId) 
        : SIPSystemException("Fund not found: " + fundId), fundId_(fundId) {}
    
    const std::string& getFundId() const { return fundId_; }
private:
    std::string fundId_;
};

/**
 * Thrown when an SIP is not found.
 */
class SIPNotFoundException : public SIPSystemException {
public:
    explicit SIPNotFoundException(const std::string& sipId) 
        : SIPSystemException("SIP not found: " + sipId), sipId_(sipId) {}
    
    const std::string& getSipId() const { return sipId_; }
private:
    std::string sipId_;
};

/**
 * Thrown when a user is not found.
 */
class UserNotFoundException : public SIPSystemException {
public:
    explicit UserNotFoundException(const std::string& userId) 
        : SIPSystemException("User not found: " + userId), userId_(userId) {}
    
    const std::string& getUserId() const { return userId_; }
private:
    std::string userId_;
};

/**
 * Thrown when an invalid state transition is attempted.
 */
class InvalidStateException : public SIPSystemException {
public:
    InvalidStateException(const std::string& sipId, const std::string& currentState, 
                          const std::string& operation)
        : SIPSystemException("Invalid operation '" + operation + "' for SIP " + sipId + 
                            " in state " + currentState),
          sipId_(sipId), currentState_(currentState), operation_(operation) {}
    
    const std::string& getSipId() const { return sipId_; }
    const std::string& getCurrentState() const { return currentState_; }
    const std::string& getOperation() const { return operation_; }
private:
    std::string sipId_;
    std::string currentState_;
    std::string operation_;
};

/**
 * Thrown when validation fails.
 */
class ValidationException : public SIPSystemException {
public:
    explicit ValidationException(const std::string& message) 
        : SIPSystemException("Validation error: " + message) {}
};

/**
 * Thrown when a transaction is not found.
 */
class TransactionNotFoundException : public SIPSystemException {
public:
    explicit TransactionNotFoundException(const std::string& transactionId) 
        : SIPSystemException("Transaction not found: " + transactionId), 
          transactionId_(transactionId) {}
    
    const std::string& getTransactionId() const { return transactionId_; }
private:
    std::string transactionId_;
};

} // namespace sip

#endif // EXCEPTIONS_H
