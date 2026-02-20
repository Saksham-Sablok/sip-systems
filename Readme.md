# SIP Management System

A C++ implementation of a Systematic Investment Plan (SIP) management system for mutual fund investments.

## Build & Run

```bash
cd sip-system
g++ -std=c++14 -Wall -Wextra -I. -o sip_system main.cpp
./sip_system
```

## Menu Options

### 1. Browse Mutual Fund Catalog
View available mutual funds for investment. You can:
- View all funds
- Filter by category (Equity, Debt, Hybrid, ELSS)
- Filter by risk level (Low, Medium, High)

Each fund displays its ID, name, category, risk level, and current NAV.

### 2. Create New SIP
Set up a new Systematic Investment Plan:
- Select a mutual fund from the catalog
- Enter investment amount (in Rs.)
- Choose frequency (Weekly, Monthly, or Quarterly)
- Optionally enable Step-Up SIP (amount increases by a percentage each installment)

### 3. View My SIPs
List all your SIPs with key details:
- Fund name and SIP ID
- Investment amount and frequency
- Current state (Active/Paused/Stopped)
- Number of installments completed
- Next execution date

### 4. Manage SIP (Pause/Unpause/Stop)
Control your SIP lifecycle:
- **Pause**: Temporarily halt SIP execution (can be resumed)
- **Unpause**: Resume a paused SIP
- **Stop**: Permanently terminate an SIP (cannot be undone)
- **Modify Step-Up**: Change the step-up percentage

### 5. View Portfolio
See your complete investment portfolio:
- Total amount invested
- Current value and units held
- Gain/Loss (absolute and percentage)
- Count of Active, Paused, and Stopped SIPs
- Filter view by SIP state

### 6. View Transaction History
View all transactions for a specific SIP:
- Transaction ID and date
- Amount invested
- Units purchased
- NAV at time of purchase
- Payment status

### 7. Execute Due SIPs (Simulate)
Process all SIPs that are due for execution on the current simulation date. This:
- Checks which SIPs have reached their execution date
- Deducts the installment amount
- Purchases units at current NAV
- Records the transaction
- Updates the next execution date

### 8. Advance Date (Simulate)
Move the simulation calendar forward to trigger future SIP executions:
- Advance by 1 day, 1 week, or 1 month
- Or enter a custom number of days

This is a simulation feature - in a real system, SIPs execute automatically on scheduled dates.

### 9. Simulate Market Movement
Change the NAV (price) of all mutual funds to simulate market conditions:
- **Market Up 5%**: Increase all fund NAVs by 5%
- **Market Up 10%**: Increase all fund NAVs by 10%
- **Market Down 5%**: Decrease all fund NAVs by 5%
- **Market Down 10%**: Decrease all fund NAVs by 10%
- **Custom percentage**: Enter any percentage (positive or negative)

Use this to see how market movements affect your portfolio value and gain/loss calculations.

## Typical Usage Flow

1. **Browse Catalog** - Explore available funds
2. **Create SIP** - Set up your investment plan
3. **Execute Due SIPs** - Process the first installment
4. **Advance Date** - Move forward in time (e.g., 1 month)
5. **Execute Due SIPs** - Process the next installment
6. **Simulate Market Movement** - Change fund prices (e.g., +10%)
7. **View Portfolio** - Check your investment performance with gains/losses
8. **Repeat steps 4-7** to simulate long-term investing

## Features

- Multiple SIP frequencies (Weekly, Monthly, Quarterly)
- Step-Up SIP support (compound growth of installment amount)
- SIP lifecycle management (Pause, Unpause, Stop)
- Portfolio tracking with gain/loss calculation
- Transaction history per SIP
- Market simulation for NAV changes
