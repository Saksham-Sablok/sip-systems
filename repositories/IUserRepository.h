#ifndef IUSER_REPOSITORY_H
#define IUSER_REPOSITORY_H

#include "IRepository.h"
#include "../models/User.h"

namespace sip {

/**
 * Repository interface for User entities.
 * Basic CRUD operations inherited from IRepository.
 */
class IUserRepository : public IRepository<User> {
public:
    virtual ~IUserRepository() = default;

    // Additional user-specific methods can be added here
    // e.g., findByEmail, findByName
    virtual std::shared_ptr<User> getByEmail(const std::string& email) const = 0;
};

} // namespace sip

#endif // IUSER_REPOSITORY_H
