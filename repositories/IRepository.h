#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace sip {

/**
 * Generic repository interface for CRUD operations.
 * Provides extensibility for future database integration.
 * 
 * Note: Uses pointer returns for optional values to maintain C++11 compatibility.
 * Returns nullptr if entity not found.
 */
template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    // Create
    virtual void add(const T& entity) = 0;

    // Read - returns shared_ptr, nullptr if not found
    virtual std::shared_ptr<T> getById(const std::string& id) const = 0;
    virtual std::vector<T> getAll() const = 0;

    // Update - returns true if entity was found and updated
    virtual bool update(const T& entity) = 0;

    // Delete - returns true if entity was found and removed
    virtual bool remove(const std::string& id) = 0;

    // Check existence
    virtual bool exists(const std::string& id) const = 0;

    // Count
    virtual size_t count() const = 0;
};

} // namespace sip

#endif // IREPOSITORY_H
