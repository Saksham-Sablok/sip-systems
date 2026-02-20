/**
 * SIP Management System - Interactive Driver Program
 * 
 * Interactive menu-driven system demonstrating all features:
 * 1. Mutual fund catalog browsing and filtering
 * 2. SIP creation with various frequencies
 * 3. Step-up SIP functionality
 * 4. SIP lifecycle management (pause, unpause, stop)
 * 5. SIP execution with real-time NAV
 * 6. Portfolio view with gain/loss calculation
 * 7. Transaction history
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <limits>

// Models
#include "models/Enums.h"
#include "models/MutualFund.h"
#include "models/User.h"
#include "models/SIP.h"
#include "models/Transaction.h"

// Repositories
#include "repositories/InMemoryMutualFundRepository.h"
#include "repositories/InMemoryUserRepository.h"
#include "repositories/InMemorySIPRepository.h"
#include "repositories/InMemoryTransactionRepository.h"

// Services
#include "services/MutualFundServiceImpl.h"
#include "services/SIPServiceImpl.h"
#include "services/PortfolioServiceImpl.h"
#include "services/MockPaymentService.h"
#include "services/MockMarketPriceService.h"

// Scheduler
#include "scheduler/SIPScheduler.h"

// Utils
#include "utils/DateUtils.h"
#include "utils/IdGenerator.h"
#include "utils/Exceptions.h"

using namespace sip;

// ============================================================================
// Global Services (initialized in main)
// ============================================================================
std::shared_ptr<IMutualFundRepository> g_fundRepo;
std::shared_ptr<IUserRepository> g_userRepo;
std::shared_ptr<ISIPRepository> g_sipRepo;
std::shared_ptr<ITransactionRepository> g_txnRepo;

std::shared_ptr<MockMarketPriceService> g_marketPriceService;
std::shared_ptr<MockPaymentService> g_paymentService;
std::shared_ptr<MutualFundServiceImpl> g_fundService;
std::shared_ptr<SIPServiceImpl> g_sipService;
std::shared_ptr<PortfolioServiceImpl> g_portfolioService;
std::shared_ptr<SIPScheduler> g_scheduler;

std::string g_currentUserId;
Date g_currentDate;

// ============================================================================
// Helper Functions
// ============================================================================

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI escape codes
}

void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printSubHeader(const std::string& title) {
    std::cout << "\n" << std::string(40, '-') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(40, '-') << std::endl;
}

int getIntInput(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input. Please enter a number between " << min << " and " << max << ".\n";
    }
}

double getDoubleInput(const std::string& prompt, double min = 0.0) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value > min) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input. Please enter a positive number.\n";
    }
}

std::string getStringInput(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

void printFundTable(const std::vector<MutualFund>& funds) {
    std::cout << "\n  " << std::left 
              << std::setw(4) << "#"
              << std::setw(14) << "Fund ID"
              << std::setw(28) << "Name"
              << std::setw(10) << "Category"
              << std::setw(8) << "Risk"
              << "NAV" << std::endl;
    std::cout << "  " << std::string(75, '-') << std::endl;
    
    int idx = 1;
    for (const auto& fund : funds) {
        // Use marketPriceService to get current NAV (reflects market movements)
        double currentNav = g_marketPriceService->getCurrentNAV(fund.getId());
        std::cout << "  " << std::left 
                  << std::setw(4) << idx++
                  << std::setw(14) << fund.getId()
                  << std::setw(28) << fund.getName()
                  << std::setw(10) << sip::toString(fund.getCategory())
                  << std::setw(8) << sip::toString(fund.getRiskLevel())
                  << "Rs. " << std::fixed << std::setprecision(2) << currentNav
                  << std::endl;
    }
}

void printSIPDetails(const SIP& sip) {
    auto fund = g_fundRepo->getById(sip.getFundId());
    std::string fundName = fund ? fund->getName() : "Unknown";
    
    std::cout << "\n  SIP Details:" << std::endl;
    std::cout << "  ------------" << std::endl;
    std::cout << "  SIP ID:           " << sip.getId() << std::endl;
    std::cout << "  Fund:             " << fundName << " (" << sip.getFundId() << ")" << std::endl;
    std::cout << "  Base Amount:      Rs. " << std::fixed << std::setprecision(2) << sip.getBaseAmount() << std::endl;
    std::cout << "  Frequency:        " << sip::toString(sip.getFrequency()) << std::endl;
    std::cout << "  State:            " << sip::toString(sip.getState()) << std::endl;
    std::cout << "  Installments:     " << sip.getInstallmentCount() << std::endl;
    std::cout << "  Step-Up:          " << sip.getStepUpPercentage() << "%" << std::endl;
    std::cout << "  Start Date:       " << DateUtils::formatDate(sip.getStartDate()) << std::endl;
    std::cout << "  Next Execution:   " << DateUtils::formatDate(sip.getNextExecutionDate()) << std::endl;
}

void printTransaction(const Transaction& txn) {
    std::cout << "  " << std::left << std::setw(15) << txn.getId()
              << "Rs. " << std::right << std::setw(10) << std::fixed << std::setprecision(2) << txn.getAmount()
              << "  Units: " << std::setw(10) << txn.getUnits()
              << "  NAV: " << std::setw(8) << txn.getNav()
              << "  " << sip::toString(txn.getStatus())
              << std::endl;
}

void printPortfolioItem(const SIPPortfolioItem& item) {
    std::cout << "\n  " << item.sip.getId() << " - " << item.fundName << std::endl;
    std::cout << "    State: " << sip::toString(item.sip.getState()) 
              << " | Frequency: " << sip::toString(item.sip.getFrequency()) << std::endl;
    std::cout << "    Invested: Rs. " << std::fixed << std::setprecision(2) << item.totalInvested
              << " | Units: " << item.totalUnits << std::endl;
    std::cout << "    Current Value: Rs. " << item.currentValue 
              << " | NAV: Rs. " << item.currentNav << std::endl;
    std::cout << "    Gain/Loss: Rs. " << item.gainLoss 
              << " (" << (item.gainLoss >= 0 ? "+" : "") << item.gainLossPercentage << "%)" << std::endl;
    if (item.sip.getStepUpPercentage() > 0) {
        std::cout << "    Step-Up: " << item.sip.getStepUpPercentage() << "% | Next Installment: Rs. " 
                  << item.nextInstallmentAmount << std::endl;
    }
}

void printPortfolioSummary(const PortfolioSummary& summary) {
    std::cout << "\n  PORTFOLIO SUMMARY" << std::endl;
    std::cout << "  -----------------" << std::endl;
    std::cout << "  Total Invested:    Rs. " << std::fixed << std::setprecision(2) << summary.totalInvested << std::endl;
    std::cout << "  Current Value:     Rs. " << summary.totalCurrentValue << std::endl;
    std::cout << "  Total Units:       " << summary.totalUnits << std::endl;
    std::cout << "  Gain/Loss:         Rs. " << summary.gainLoss 
              << " (" << (summary.gainLoss >= 0 ? "+" : "") << summary.gainLossPercentage << "%)" << std::endl;
    std::cout << "  Active SIPs:       " << summary.activeSIPCount << std::endl;
    std::cout << "  Paused SIPs:       " << summary.pausedSIPCount << std::endl;
    std::cout << "  Stopped SIPs:      " << summary.stoppedSIPCount << std::endl;
}

// ============================================================================
// Menu Functions
// ============================================================================

void showMainMenu() {
    std::cout << "\n  Current Date: " << DateUtils::formatDate(g_currentDate) << std::endl;
    std::cout << "\n  MAIN MENU" << std::endl;
    std::cout << "  ---------" << std::endl;
    std::cout << "  1. Browse Mutual Fund Catalog" << std::endl;
    std::cout << "  2. Create New SIP" << std::endl;
    std::cout << "  3. View My SIPs" << std::endl;
    std::cout << "  4. Manage SIP (Pause/Unpause/Stop)" << std::endl;
    std::cout << "  5. View Portfolio" << std::endl;
    std::cout << "  6. View Transaction History" << std::endl;
    std::cout << "  7. Execute Due SIPs (Simulate)" << std::endl;
    std::cout << "  8. Advance Date (Simulate)" << std::endl;
    std::cout << "  9. Simulate Market Movement" << std::endl;
    std::cout << "  0. Exit" << std::endl;
}

void browseFundCatalog() {
    printHeader("MUTUAL FUND CATALOG");
    
    std::cout << "\n  Filter Options:" << std::endl;
    std::cout << "  1. View All Funds" << std::endl;
    std::cout << "  2. Filter by Category" << std::endl;
    std::cout << "  3. Filter by Risk Level" << std::endl;
    std::cout << "  0. Back to Main Menu" << std::endl;
    
    int choice = getIntInput("\n  Select option: ", 0, 3);
    
    std::vector<MutualFund> funds;
    
    switch (choice) {
        case 1:
            printSubHeader("All Mutual Funds");
            funds = g_fundService->getAllFunds();
            break;
        case 2: {
            std::cout << "\n  Select Category:" << std::endl;
            std::cout << "  1. EQUITY" << std::endl;
            std::cout << "  2. DEBT" << std::endl;
            std::cout << "  3. HYBRID" << std::endl;
            std::cout << "  4. ELSS" << std::endl;
            int cat = getIntInput("  Choice: ", 1, 4);
            FundCategory category = static_cast<FundCategory>(cat - 1);
            printSubHeader(std::string("Funds - ") + sip::toString(category));
            funds = g_fundService->filterByCategory(category);
            break;
        }
        case 3: {
            std::cout << "\n  Select Risk Level:" << std::endl;
            std::cout << "  1. LOW" << std::endl;
            std::cout << "  2. MEDIUM" << std::endl;
            std::cout << "  3. HIGH" << std::endl;
            int risk = getIntInput("  Choice: ", 1, 3);
            RiskLevel riskLevel = static_cast<RiskLevel>(risk - 1);
            printSubHeader(std::string("Funds - ") + sip::toString(riskLevel) + " Risk");
            funds = g_fundService->filterByRiskLevel(riskLevel);
            break;
        }
        case 0:
            return;
    }
    
    if (funds.empty()) {
        std::cout << "\n  No funds found matching the criteria." << std::endl;
    } else {
        printFundTable(funds);
    }
    
    waitForEnter();
}

void createSIP() {
    printHeader("CREATE NEW SIP");
    
    // Show available funds
    auto funds = g_fundService->getAllFunds();
    std::cout << "\n  Available Funds:" << std::endl;
    printFundTable(funds);
    
    // Select fund
    int fundChoice = getIntInput("\n  Select fund number: ", 1, static_cast<int>(funds.size()));
    std::string fundId = funds[fundChoice - 1].getId();
    
    // Enter amount
    double amount = getDoubleInput("  Enter SIP amount (Rs.): ");
    
    // Select frequency
    std::cout << "\n  Select Frequency:" << std::endl;
    std::cout << "  1. Weekly" << std::endl;
    std::cout << "  2. Monthly" << std::endl;
    std::cout << "  3. Quarterly" << std::endl;
    int freqChoice = getIntInput("  Choice: ", 1, 3);
    SIPFrequency frequency;
    switch (freqChoice) {
        case 1: frequency = SIPFrequency::WEEKLY; break;
        case 2: frequency = SIPFrequency::MONTHLY; break;
        case 3: frequency = SIPFrequency::QUARTERLY; break;
        default: frequency = SIPFrequency::MONTHLY;
    }
    
    // Step-up option
    std::cout << "\n  Enable Step-Up SIP? (increases amount each installment)" << std::endl;
    std::cout << "  1. No Step-Up" << std::endl;
    std::cout << "  2. Yes, enable Step-Up" << std::endl;
    int stepUpChoice = getIntInput("  Choice: ", 1, 2);
    
    double stepUpPercentage = 0.0;
    if (stepUpChoice == 2) {
        stepUpPercentage = getDoubleInput("  Enter step-up percentage (e.g., 10 for 10%): ");
    }
    
    // Confirm
    std::cout << "\n  SIP Summary:" << std::endl;
    std::cout << "  Fund: " << funds[fundChoice - 1].getName() << std::endl;
    std::cout << "  Amount: Rs. " << std::fixed << std::setprecision(2) << amount << std::endl;
    std::cout << "  Frequency: " << sip::toString(frequency) << std::endl;
    std::cout << "  Step-Up: " << stepUpPercentage << "%" << std::endl;
    std::cout << "  Start Date: " << DateUtils::formatDate(g_currentDate) << std::endl;
    
    std::cout << "\n  Confirm creation? (1=Yes, 0=No): ";
    int confirm = getIntInput("", 0, 1);
    
    if (confirm == 1) {
        try {
            SIP sip = g_sipService->createSIP(g_currentUserId, fundId, amount, frequency, g_currentDate, stepUpPercentage);
            std::cout << "\n  SUCCESS! SIP created." << std::endl;
            printSIPDetails(sip);
        } catch (const std::exception& e) {
            std::cout << "\n  ERROR: " << e.what() << std::endl;
        }
    } else {
        std::cout << "\n  SIP creation cancelled." << std::endl;
    }
    
    waitForEnter();
}

void viewMySIPs() {
    printHeader("MY SIPs");
    
    auto sips = g_sipService->getSIPsByUser(g_currentUserId);
    
    if (sips.empty()) {
        std::cout << "\n  You don't have any SIPs yet." << std::endl;
        std::cout << "  Go to 'Create New SIP' to start investing!" << std::endl;
    } else {
        std::cout << "\n  You have " << sips.size() << " SIP(s):" << std::endl;
        
        int idx = 1;
        for (const auto& sip : sips) {
            auto fund = g_fundRepo->getById(sip.getFundId());
            std::string fundName = fund ? fund->getName() : "Unknown";
            
            std::cout << "\n  " << idx++ << ". " << sip.getId() << " - " << fundName << std::endl;
            std::cout << "     Amount: Rs. " << std::fixed << std::setprecision(2) << sip.getBaseAmount()
                      << " | " << sip::toString(sip.getFrequency()) 
                      << " | State: " << sip::toString(sip.getState()) << std::endl;
            if (sip.getStepUpPercentage() > 0) {
                std::cout << "     Step-Up: " << sip.getStepUpPercentage() << "%" << std::endl;
            }
            std::cout << "     Installments: " << sip.getInstallmentCount() 
                      << " | Next: " << DateUtils::formatDate(sip.getNextExecutionDate()) << std::endl;
        }
    }
    
    waitForEnter();
}

void manageSIP() {
    printHeader("MANAGE SIP");
    
    auto sips = g_sipService->getSIPsByUser(g_currentUserId);
    
    if (sips.empty()) {
        std::cout << "\n  You don't have any SIPs to manage." << std::endl;
        waitForEnter();
        return;
    }
    
    // Show SIPs
    std::cout << "\n  Your SIPs:" << std::endl;
    int idx = 1;
    for (const auto& sip : sips) {
        auto fund = g_fundRepo->getById(sip.getFundId());
        std::string fundName = fund ? fund->getName() : "Unknown";
        std::cout << "  " << idx++ << ". " << sip.getId() << " - " << fundName 
                  << " [" << sip::toString(sip.getState()) << "]" << std::endl;
    }
    std::cout << "  0. Back" << std::endl;
    
    int sipChoice = getIntInput("\n  Select SIP: ", 0, static_cast<int>(sips.size()));
    if (sipChoice == 0) return;
    
    SIP& selectedSip = sips[sipChoice - 1];
    printSIPDetails(selectedSip);
    
    std::cout << "\n  Actions:" << std::endl;
    std::cout << "  1. Pause SIP" << std::endl;
    std::cout << "  2. Unpause SIP" << std::endl;
    std::cout << "  3. Stop SIP" << std::endl;
    std::cout << "  4. Modify Step-Up Percentage" << std::endl;
    std::cout << "  0. Back" << std::endl;
    
    int action = getIntInput("\n  Select action: ", 0, 4);
    
    try {
        switch (action) {
            case 1:
                g_sipService->pauseSIP(selectedSip.getId());
                std::cout << "\n  SUCCESS! SIP paused." << std::endl;
                break;
            case 2:
                g_sipService->unpauseSIP(selectedSip.getId());
                std::cout << "\n  SUCCESS! SIP unpaused." << std::endl;
                break;
            case 3:
                std::cout << "\n  WARNING: Stopping an SIP is permanent. Continue? (1=Yes, 0=No): ";
                if (getIntInput("", 0, 1) == 1) {
                    g_sipService->stopSIP(selectedSip.getId());
                    std::cout << "\n  SUCCESS! SIP stopped." << std::endl;
                } else {
                    std::cout << "\n  Cancelled." << std::endl;
                }
                break;
            case 4: {
                double newStepUp = getDoubleInput("  Enter new step-up percentage: ", -0.01);
                g_sipService->modifyStepUp(selectedSip.getId(), newStepUp);
                std::cout << "\n  SUCCESS! Step-up updated to " << newStepUp << "%" << std::endl;
                break;
            }
            case 0:
                return;
        }
        
        // Show updated SIP
        if (action >= 1 && action <= 4) {
            SIP updated = g_sipService->getSIPById(selectedSip.getId());
            printSIPDetails(updated);
        }
    } catch (const std::exception& e) {
        std::cout << "\n  ERROR: " << e.what() << std::endl;
    }
    
    waitForEnter();
}

void viewPortfolio() {
    printHeader("MY PORTFOLIO");
    
    auto portfolio = g_portfolioService->getUserPortfolio(g_currentUserId);
    
    if (portfolio.empty()) {
        std::cout << "\n  Your portfolio is empty." << std::endl;
        std::cout << "  Create an SIP and execute it to see your investments." << std::endl;
    } else {
        // Show summary first
        auto summary = g_portfolioService->getPortfolioSummary(g_currentUserId);
        printPortfolioSummary(summary);
        
        // Filter options
        std::cout << "\n  View Options:" << std::endl;
        std::cout << "  1. All SIPs" << std::endl;
        std::cout << "  2. Active SIPs only" << std::endl;
        std::cout << "  3. Paused SIPs only" << std::endl;
        std::cout << "  4. Stopped SIPs only" << std::endl;
        
        int choice = getIntInput("\n  Select: ", 1, 4);
        
        std::vector<SIPPortfolioItem> items;
        switch (choice) {
            case 1: items = portfolio; break;
            case 2: items = g_portfolioService->filterByState(g_currentUserId, SIPState::ACTIVE); break;
            case 3: items = g_portfolioService->filterByState(g_currentUserId, SIPState::PAUSED); break;
            case 4: items = g_portfolioService->filterByState(g_currentUserId, SIPState::STOPPED); break;
        }
        
        if (items.empty()) {
            std::cout << "\n  No SIPs found with the selected filter." << std::endl;
        } else {
            printSubHeader("SIP Details");
            for (const auto& item : items) {
                printPortfolioItem(item);
            }
        }
    }
    
    waitForEnter();
}

void viewTransactionHistory() {
    printHeader("TRANSACTION HISTORY");
    
    auto sips = g_sipService->getSIPsByUser(g_currentUserId);
    
    if (sips.empty()) {
        std::cout << "\n  You don't have any SIPs." << std::endl;
        waitForEnter();
        return;
    }
    
    // Show SIPs
    std::cout << "\n  Select SIP to view transactions:" << std::endl;
    int idx = 1;
    for (const auto& sip : sips) {
        auto fund = g_fundRepo->getById(sip.getFundId());
        std::string fundName = fund ? fund->getName() : "Unknown";
        std::cout << "  " << idx++ << ". " << sip.getId() << " - " << fundName << std::endl;
    }
    std::cout << "  0. Back" << std::endl;
    
    int choice = getIntInput("\n  Select: ", 0, static_cast<int>(sips.size()));
    if (choice == 0) return;
    
    std::string sipId = sips[choice - 1].getId();
    auto transactions = g_portfolioService->getTransactionHistory(sipId);
    
    if (transactions.empty()) {
        std::cout << "\n  No transactions yet for this SIP." << std::endl;
        std::cout << "  Execute due SIPs to generate transactions." << std::endl;
    } else {
        printSubHeader("Transactions for " + sipId);
        std::cout << "\n  " << std::left << std::setw(15) << "Transaction ID"
                  << std::setw(14) << "Amount"
                  << std::setw(16) << "Units"
                  << std::setw(12) << "NAV"
                  << "Status" << std::endl;
        std::cout << "  " << std::string(60, '-') << std::endl;
        
        double totalAmount = 0, totalUnits = 0;
        for (const auto& txn : transactions) {
            printTransaction(txn);
            if (txn.getStatus() == PaymentStatus::SUCCESS) {
                totalAmount += txn.getAmount();
                totalUnits += txn.getUnits();
            }
        }
        
        std::cout << "  " << std::string(60, '-') << std::endl;
        std::cout << "  Total: Rs. " << std::fixed << std::setprecision(2) << totalAmount
                  << " | Units: " << totalUnits << std::endl;
    }
    
    waitForEnter();
}

void executeDueSIPs() {
    printHeader("EXECUTE DUE SIPs");
    
    std::cout << "\n  Current Date: " << DateUtils::formatDate(g_currentDate) << std::endl;
    std::cout << "\n  This will execute all SIPs that are due on or before the current date." << std::endl;
    std::cout << "  Proceed? (1=Yes, 0=No): ";
    
    if (getIntInput("", 0, 1) == 0) {
        std::cout << "\n  Cancelled." << std::endl;
        waitForEnter();
        return;
    }
    
    std::cout << "\n  Executing SIPs..." << std::endl;
    
    int processed = g_scheduler->executeDueSIPs(g_currentDate);
    
    std::cout << "\n  RESULT: " << processed << " SIP(s) processed." << std::endl;
    
    if (processed > 0) {
        std::cout << "\n  Recent Transactions:" << std::endl;
        auto allTxns = g_txnRepo->getAll();
        int count = 0;
        for (auto it = allTxns.rbegin(); it != allTxns.rend() && count < processed; ++it, ++count) {
            std::cout << "    - " << it->getId() << ": Rs. " << std::fixed << std::setprecision(2) 
                      << it->getAmount() << " -> " << it->getUnits() << " units @ NAV " 
                      << it->getNav() << std::endl;
        }
    }
    
    waitForEnter();
}

void advanceDate() {
    printHeader("ADVANCE DATE (SIMULATION)");
    
    std::cout << "\n  Current Date: " << DateUtils::formatDate(g_currentDate) << std::endl;
    std::cout << "\n  Advance by:" << std::endl;
    std::cout << "  1. 1 Day" << std::endl;
    std::cout << "  2. 1 Week" << std::endl;
    std::cout << "  3. 1 Month" << std::endl;
    std::cout << "  4. Custom days" << std::endl;
    std::cout << "  0. Back" << std::endl;
    
    int choice = getIntInput("\n  Select: ", 0, 4);
    
    int days = 0;
    switch (choice) {
        case 1: days = 1; break;
        case 2: days = 7; break;
        case 3: days = 30; break;
        case 4: days = getIntInput("  Enter number of days: ", 1, 365); break;
        case 0: return;
    }
    
    g_currentDate = g_currentDate + std::chrono::hours(24 * days);
    
    std::cout << "\n  Date advanced to: " << DateUtils::formatDate(g_currentDate) << std::endl;
    
    // Check for due SIPs
    auto dueSips = g_sipRepo->getDueSIPs(g_currentDate);
    if (!dueSips.empty()) {
        std::cout << "\n  NOTE: " << dueSips.size() << " SIP(s) are now due for execution!" << std::endl;
        std::cout << "  Use 'Execute Due SIPs' to process them." << std::endl;
    }
    
    waitForEnter();
}

void simulateMarketMovement() {
    printHeader("SIMULATE MARKET MOVEMENT");
    
    std::cout << "\n  This simulates a market-wide NAV change for all funds." << std::endl;
    std::cout << "\n  Current Fund NAVs:" << std::endl;
    auto funds = g_fundService->getAllFunds();
    for (const auto& fund : funds) {
        std::cout << "    " << fund.getId() << ": Rs. " << std::fixed << std::setprecision(2) 
                  << g_marketPriceService->getStoredNAV(fund.getId()) << std::endl;
    }
    
    std::cout << "\n  Select movement:" << std::endl;
    std::cout << "  1. Market Up 5%" << std::endl;
    std::cout << "  2. Market Up 10%" << std::endl;
    std::cout << "  3. Market Down 5%" << std::endl;
    std::cout << "  4. Market Down 10%" << std::endl;
    std::cout << "  5. Custom percentage" << std::endl;
    std::cout << "  0. Back" << std::endl;
    
    int choice = getIntInput("\n  Select: ", 0, 5);
    
    double percentage = 0;
    switch (choice) {
        case 1: percentage = 0.05; break;
        case 2: percentage = 0.10; break;
        case 3: percentage = -0.05; break;
        case 4: percentage = -0.10; break;
        case 5: {
            std::cout << "  Enter percentage (e.g., 5 for +5%, -3 for -3%): ";
            double pct;
            std::cin >> pct;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            percentage = pct / 100.0;
            break;
        }
        case 0: return;
    }
    
    g_marketPriceService->simulateMarketMovement(percentage);
    
    // Also update the MutualFund objects in the repository to keep data consistent
    for (const auto& fund : funds) {
        double newNav = g_marketPriceService->getStoredNAV(fund.getId());
        MutualFund updatedFund = fund;
        updatedFund.setNav(newNav);
        g_fundRepo->update(updatedFund);
    }
    
    std::cout << "\n  Market moved by " << (percentage >= 0 ? "+" : "") 
              << (percentage * 100) << "%" << std::endl;
    std::cout << "\n  Updated Fund NAVs:" << std::endl;
    for (const auto& fund : funds) {
        std::cout << "    " << fund.getId() << ": Rs. " << std::fixed << std::setprecision(2) 
                  << g_marketPriceService->getStoredNAV(fund.getId()) << std::endl;
    }
    
    waitForEnter();
}

// ============================================================================
// Setup Functions
// ============================================================================

void setupSampleFunds() {
    g_fundService->addFund(MutualFund("FUND_000001", "HDFC Flexi Cap Fund", FundCategory::EQUITY, RiskLevel::HIGH, 150.50));
    g_fundService->addFund(MutualFund("FUND_000002", "ICICI Prudential Balanced", FundCategory::HYBRID, RiskLevel::MEDIUM, 85.25));
    g_fundService->addFund(MutualFund("FUND_000003", "SBI Debt Fund", FundCategory::DEBT, RiskLevel::LOW, 45.80));
    g_fundService->addFund(MutualFund("FUND_000004", "Axis ELSS Tax Saver", FundCategory::ELSS, RiskLevel::HIGH, 120.00));
    g_fundService->addFund(MutualFund("FUND_000005", "Kotak Small Cap Fund", FundCategory::EQUITY, RiskLevel::HIGH, 95.75));
    g_fundService->addFund(MutualFund("FUND_000006", "HDFC Corporate Bond", FundCategory::DEBT, RiskLevel::LOW, 32.50));
    
    g_marketPriceService->updateNAV("FUND_000001", 150.50);
    g_marketPriceService->updateNAV("FUND_000002", 85.25);
    g_marketPriceService->updateNAV("FUND_000003", 45.80);
    g_marketPriceService->updateNAV("FUND_000004", 120.00);
    g_marketPriceService->updateNAV("FUND_000005", 95.75);
    g_marketPriceService->updateNAV("FUND_000006", 32.50);
}

void setupUser() {
    std::string name = getStringInput("\n  Enter your name: ");
    if (name.empty()) name = "Demo User";
    
    std::string email = getStringInput("  Enter your email: ");
    if (email.empty()) email = "demo@example.com";
    
    g_currentUserId = IdGenerator::generateUserId();
    User user(g_currentUserId, name, email);
    g_userRepo->add(user);
    
    std::cout << "\n  Welcome, " << name << "!" << std::endl;
    std::cout << "  Your User ID: " << g_currentUserId << std::endl;
}

void initializeSystem() {
    // Reset ID generator
    IdGenerator::reset();
    
    // Initialize repositories
    g_fundRepo = std::make_shared<InMemoryMutualFundRepository>();
    g_userRepo = std::make_shared<InMemoryUserRepository>();
    g_sipRepo = std::make_shared<InMemorySIPRepository>();
    g_txnRepo = std::make_shared<InMemoryTransactionRepository>();
    
    // Initialize services
    g_marketPriceService = std::make_shared<MockMarketPriceService>(false, 0.0);
    g_paymentService = std::make_shared<MockPaymentService>(1.0, true);
    g_fundService = std::make_shared<MutualFundServiceImpl>(g_fundRepo);
    g_sipService = std::make_shared<SIPServiceImpl>(g_sipRepo, g_userRepo, g_fundService);
    g_portfolioService = std::make_shared<PortfolioServiceImpl>(g_sipRepo, g_txnRepo, g_fundRepo, g_marketPriceService);
    
    // Initialize scheduler
    g_scheduler = std::make_shared<SIPScheduler>(g_sipRepo, g_txnRepo, g_marketPriceService, g_paymentService, g_sipService);
    
    // Set current date
    g_currentDate = DateUtils::createDate(2024, 1, 1);
    
    // Setup sample funds
    setupSampleFunds();
}

// ============================================================================
// Main
// ============================================================================

int main() {
    clearScreen();
    
    std::cout << "\n";
    std::cout << "  ========================================================" << std::endl;
    std::cout << "           SIP MANAGEMENT SYSTEM" << std::endl;
    std::cout << "         Systematic Investment Plans" << std::endl;
    std::cout << "  ========================================================" << std::endl;
    
    // Initialize system
    initializeSystem();
    
    std::cout << "\n  System initialized with 6 mutual funds." << std::endl;
    std::cout << "  Starting date: " << DateUtils::formatDate(g_currentDate) << std::endl;
    
    // Setup user
    setupUser();
    
    waitForEnter();
    
    // Main loop
    bool running = true;
    while (running) {
        clearScreen();
        printHeader("SIP MANAGEMENT SYSTEM");
        showMainMenu();
        
        int choice = getIntInput("\n  Select option: ", 0, 9);
        
        switch (choice) {
            case 1: browseFundCatalog(); break;
            case 2: createSIP(); break;
            case 3: viewMySIPs(); break;
            case 4: manageSIP(); break;
            case 5: viewPortfolio(); break;
            case 6: viewTransactionHistory(); break;
            case 7: executeDueSIPs(); break;
            case 8: advanceDate(); break;
            case 9: simulateMarketMovement(); break;
            case 0:
                std::cout << "\n  Thank you for using SIP Management System!" << std::endl;
                std::cout << "  Goodbye!" << std::endl;
                running = false;
                break;
        }
    }
    
    return 0;
}
