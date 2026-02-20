#ifndef MUTUAL_FUND_H
#define MUTUAL_FUND_H

#include <string>
#include "Enums.h"

namespace sip {

class MutualFund {
private:
    std::string id;
    std::string name;
    FundCategory category;
    RiskLevel riskLevel;
    double nav;  // Net Asset Value

public:
    MutualFund() : category(FundCategory::EQUITY), riskLevel(RiskLevel::MEDIUM), nav(0.0) {}
    
    MutualFund(const std::string& id, const std::string& name, 
               FundCategory category, RiskLevel riskLevel, double nav)
        : id(id), name(name), category(category), riskLevel(riskLevel), nav(nav) {}

    // Getters
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    FundCategory getCategory() const { return category; }
    RiskLevel getRiskLevel() const { return riskLevel; }
    double getNav() const { return nav; }

    // Setters
    void setId(const std::string& id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setCategory(FundCategory category) { this->category = category; }
    void setRiskLevel(RiskLevel riskLevel) { this->riskLevel = riskLevel; }
    void setNav(double nav) { this->nav = nav; }

    // Display helper
    std::string toString() const {
        return "MutualFund{id=" + id + ", name=" + name + 
               ", category=" + sip::toString(category) + 
               ", riskLevel=" + sip::toString(riskLevel) + 
               ", nav=" + std::to_string(nav) + "}";
    }
};

} // namespace sip

#endif // MUTUAL_FUND_H
