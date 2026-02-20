#ifndef ID_GENERATOR_H
#define ID_GENERATOR_H

#include <string>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace sip {

/**
 * Thread-safe ID generator using atomic counter.
 * Generates unique IDs with optional prefix.
 */
class IdGenerator {
private:
    static std::atomic<uint64_t> counter;

public:

    /**
     * Generate a simple sequential ID with prefix.
     * Format: PREFIX_COUNTER
     */
    static std::string generateSimple(const std::string& prefix = "ID") {
        uint64_t count = counter.fetch_add(1);
        std::ostringstream oss;
        oss << prefix << "_" << std::setfill('0') << std::setw(6) << count;
        return oss.str();
    }

    /**
     * Generate ID for specific entity types.
     */
    static std::string generateFundId() { return generateSimple("FUND"); }
    static std::string generateUserId() { return generateSimple("USER"); }
    static std::string generateSipId() { return generateSimple("SIP"); }
    static std::string generateTransactionId() { return generateSimple("TXN"); }

    /**
     * Reset counter (mainly for testing purposes).
     */
    static void reset() {
        counter = 1;
    }
};

// Initialize static counter
std::atomic<uint64_t> IdGenerator::counter{1};

} // namespace sip

#endif // ID_GENERATOR_H
