#ifndef INMEMORY_USER_REPOSITORY_H
#define INMEMORY_USER_REPOSITORY_H

#include "IUserRepository.h"
#include <unordered_map>

namespace sip {

/**
 * In-memory implementation of IUserRepository.
 * Uses unordered_map for O(1) lookups by ID.
 */
class InMemoryUserRepository : public IUserRepository {
private:
    std::unordered_map<std::string, User> storage;
    std::unordered_map<std::string, std::string> emailIndex; // email -> userId

public:
    void add(const User& user) override {
        storage[user.getId()] = user;
        emailIndex[user.getEmail()] = user.getId();
    }

    std::shared_ptr<User> getById(const std::string& id) const override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            return std::make_shared<User>(it->second);
        }
        return nullptr;
    }

    std::vector<User> getAll() const override {
        std::vector<User> result;
        result.reserve(storage.size());
        for (const auto& pair : storage) {
            result.push_back(pair.second);
        }
        return result;
    }

    bool update(const User& user) override {
        auto it = storage.find(user.getId());
        if (it != storage.end()) {
            // Remove old email from index
            emailIndex.erase(it->second.getEmail());
            // Update user
            it->second = user;
            // Add new email to index
            emailIndex[user.getEmail()] = user.getId();
            return true;
        }
        return false;
    }

    bool remove(const std::string& id) override {
        auto it = storage.find(id);
        if (it != storage.end()) {
            emailIndex.erase(it->second.getEmail());
            storage.erase(it);
            return true;
        }
        return false;
    }

    bool exists(const std::string& id) const override {
        return storage.find(id) != storage.end();
    }

    size_t count() const override {
        return storage.size();
    }

    std::shared_ptr<User> getByEmail(const std::string& email) const override {
        auto it = emailIndex.find(email);
        if (it != emailIndex.end()) {
            return getById(it->second);
        }
        return nullptr;
    }
};

} // namespace sip

#endif // INMEMORY_USER_REPOSITORY_H
