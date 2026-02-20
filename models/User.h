#ifndef USER_H
#define USER_H

#include <string>

namespace sip {

class User {
private:
    std::string id;
    std::string name;
    std::string email;

public:
    User() = default;
    
    User(const std::string& id, const std::string& name, const std::string& email)
        : id(id), name(name), email(email) {}

    // Getters
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getEmail() const { return email; }

    // Setters
    void setId(const std::string& id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setEmail(const std::string& email) { this->email = email; }

    // Display helper
    std::string toString() const {
        return "User{id=" + id + ", name=" + name + ", email=" + email + "}";
    }
};

} // namespace sip

#endif // USER_H
