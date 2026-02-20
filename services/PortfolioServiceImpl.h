#ifndef PORTFOLIO_SERVICE_IMPL_H
#define PORTFOLIO_SERVICE_IMPL_H

#include "IPortfolioService.h"
#include "IMarketPriceService.h"
#include "../repositories/ISIPRepository.h"
#include "../repositories/ITransactionRepository.h"
#include "../repositories/IMutualFundRepository.h"
#include "../utils/Exceptions.h"
#include <memory>
#include <cmath>

namespace sip {

/**
 * Implementation of IPortfolioService.
 * Provides portfolio view and analytics operations.
 */
class PortfolioServiceImpl : public IPortfolioService {
private:
    std::shared_ptr<ISIPRepository> sipRepository;
    std::shared_ptr<ITransactionRepository> transactionRepository;
    std::shared_ptr<IMutualFundRepository> fundRepository;
    std::shared_ptr<IMarketPriceService> marketPriceService;

    /**
     * Calculate stepped-up amount using compound growth formula.
     * Formula: baseAmount * (1 + stepUpPercentage/100)^(installmentNumber - 1)
     */
    static double calculateSteppedUpAmount(double baseAmount, double stepUpPercentage, int installmentNumber) {
        if (stepUpPercentage <= 0 || installmentNumber <= 1) {
            return baseAmount;
        }
        double factor = std::pow(1.0 + stepUpPercentage / 100.0, installmentNumber - 1);
        return baseAmount * factor;
    }

    /**
     * Build a SIPPortfolioItem from an SIP and its transactions.
     */
    SIPPortfolioItem buildPortfolioItem(const SIP& sip) const {
        SIPPortfolioItem item;
        item.sip = sip;

        // Get fund name
        auto fund = fundRepository->getById(sip.getFundId());
        if (fund) {
            item.fundName = fund->getName();
        } else {
            item.fundName = "Unknown Fund";
        }

        // Get current NAV
        try {
            item.currentNav = marketPriceService->getCurrentNAV(sip.getFundId());
        } catch (const std::exception&) {
            item.currentNav = 0.0;
        }

        // Calculate totals from successful transactions
        std::vector<Transaction> transactions = transactionRepository->getSuccessfulBySipId(sip.getId());
        
        item.totalInvested = 0.0;
        item.totalUnits = 0.0;
        
        for (const auto& txn : transactions) {
            item.totalInvested += txn.getAmount();
            item.totalUnits += txn.getUnits();
        }

        // Calculate current value
        item.currentValue = item.totalUnits * item.currentNav;

        // Calculate gain/loss
        item.gainLoss = item.currentValue - item.totalInvested;
        if (item.totalInvested > 0) {
            item.gainLossPercentage = (item.gainLoss / item.totalInvested) * 100.0;
        } else {
            item.gainLossPercentage = 0.0;
        }

        // Calculate current installment amount (for next payment)
        int nextInstallment = sip.getInstallmentCount() + 1;
        item.currentInstallmentAmount = calculateSteppedUpAmount(
            sip.getBaseAmount(), sip.getStepUpPercentage(), nextInstallment);

        // Calculate next installment amount (one after current)
        item.nextInstallmentAmount = calculateSteppedUpAmount(
            sip.getBaseAmount(), sip.getStepUpPercentage(), nextInstallment + 1);

        return item;
    }

public:
    PortfolioServiceImpl(std::shared_ptr<ISIPRepository> sipRepo,
                         std::shared_ptr<ITransactionRepository> txnRepo,
                         std::shared_ptr<IMutualFundRepository> fundRepo,
                         std::shared_ptr<IMarketPriceService> marketSvc)
        : sipRepository(std::move(sipRepo)),
          transactionRepository(std::move(txnRepo)),
          fundRepository(std::move(fundRepo)),
          marketPriceService(std::move(marketSvc)) {}

    std::vector<SIPPortfolioItem> getUserPortfolio(const std::string& userId) const override {
        std::vector<SIPPortfolioItem> portfolio;
        std::vector<SIP> sips = sipRepository->getByUserId(userId);
        
        for (const auto& sip : sips) {
            portfolio.push_back(buildPortfolioItem(sip));
        }
        
        return portfolio;
    }

    PortfolioSummary getPortfolioSummary(const std::string& userId) const override {
        PortfolioSummary summary;
        std::vector<SIPPortfolioItem> portfolio = getUserPortfolio(userId);
        
        for (const auto& item : portfolio) {
            summary.totalInvested += item.totalInvested;
            summary.totalCurrentValue += item.currentValue;
            summary.totalUnits += item.totalUnits;
            
            switch (item.sip.getState()) {
                case SIPState::ACTIVE:
                    summary.activeSIPCount++;
                    break;
                case SIPState::PAUSED:
                    summary.pausedSIPCount++;
                    break;
                case SIPState::STOPPED:
                    summary.stoppedSIPCount++;
                    break;
            }
        }
        
        // Calculate overall gain/loss
        summary.gainLoss = summary.totalCurrentValue - summary.totalInvested;
        if (summary.totalInvested > 0) {
            summary.gainLossPercentage = (summary.gainLoss / summary.totalInvested) * 100.0;
        }
        
        return summary;
    }

    std::vector<SIPPortfolioItem> filterByState(const std::string& userId, SIPState state) const override {
        std::vector<SIPPortfolioItem> portfolio;
        std::vector<SIP> sips = sipRepository->getByUserIdAndState(userId, state);
        
        for (const auto& sip : sips) {
            portfolio.push_back(buildPortfolioItem(sip));
        }
        
        return portfolio;
    }

    std::vector<Transaction> getTransactionHistory(const std::string& sipId) const override {
        return transactionRepository->getBySipId(sipId);
    }

    double calculateTotalInvested(const std::string& sipId) const override {
        std::vector<Transaction> transactions = transactionRepository->getSuccessfulBySipId(sipId);
        
        double total = 0.0;
        for (const auto& txn : transactions) {
            total += txn.getAmount();
        }
        
        return total;
    }

    double calculateTotalUnits(const std::string& sipId) const override {
        std::vector<Transaction> transactions = transactionRepository->getSuccessfulBySipId(sipId);
        
        double total = 0.0;
        for (const auto& txn : transactions) {
            total += txn.getUnits();
        }
        
        return total;
    }

    double calculateCurrentValue(const std::string& sipId) const override {
        // Get SIP to find fund ID
        auto sip = sipRepository->getById(sipId);
        if (!sip) {
            throw SIPNotFoundException(sipId);
        }
        
        double totalUnits = calculateTotalUnits(sipId);
        double currentNav = marketPriceService->getCurrentNAV(sip->getFundId());
        
        return totalUnits * currentNav;
    }
};

} // namespace sip

#endif // PORTFOLIO_SERVICE_IMPL_H
