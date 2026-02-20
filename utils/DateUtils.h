#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <chrono>
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

namespace sip {

using Date = std::chrono::system_clock::time_point;

/**
 * Utility class for date operations.
 */
class DateUtils {
public:
    /**
     * Get current date/time.
     */
    static Date now() {
        return std::chrono::system_clock::now();
    }

    /**
     * Create a date from year, month, day.
     */
    static Date createDate(int year, int month, int day) {
        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        std::time_t time = std::mktime(&tm);
        return std::chrono::system_clock::from_time_t(time);
    }

    /**
     * Add weeks to a date.
     */
    static Date addWeeks(Date date, int weeks) {
        return date + std::chrono::hours(24 * 7 * weeks);
    }

    /**
     * Add months to a date.
     * Handles edge cases like 31st -> 28th/30th.
     */
    static Date addMonths(Date date, int months) {
        std::time_t time = std::chrono::system_clock::to_time_t(date);
        std::tm* tm = std::localtime(&time);
        
        int originalDay = tm->tm_mday;
        
        // Add months
        tm->tm_mon += months;
        
        // Normalize year/month overflow
        while (tm->tm_mon > 11) {
            tm->tm_mon -= 12;
            tm->tm_year++;
        }
        while (tm->tm_mon < 0) {
            tm->tm_mon += 12;
            tm->tm_year--;
        }
        
        // Handle day overflow (e.g., Jan 31 + 1 month = Feb 28/29)
        int daysInMonth = getDaysInMonth(tm->tm_year + 1900, tm->tm_mon + 1);
        if (originalDay > daysInMonth) {
            tm->tm_mday = daysInMonth;
        } else {
            tm->tm_mday = originalDay;
        }
        
        return std::chrono::system_clock::from_time_t(std::mktime(tm));
    }

    /**
     * Add quarters (3 months) to a date.
     */
    static Date addQuarters(Date date, int quarters) {
        return addMonths(date, quarters * 3);
    }

    /**
     * Get day of week (0 = Sunday, 1 = Monday, ..., 6 = Saturday).
     */
    static int getDayOfWeek(Date date) {
        std::time_t time = std::chrono::system_clock::to_time_t(date);
        std::tm* tm = std::localtime(&time);
        return tm->tm_wday;
    }

    /**
     * Get day of month (1-31).
     */
    static int getDayOfMonth(Date date) {
        std::time_t time = std::chrono::system_clock::to_time_t(date);
        std::tm* tm = std::localtime(&time);
        return tm->tm_mday;
    }

    /**
     * Check if two dates are the same day.
     */
    static bool isSameDay(Date date1, Date date2) {
        std::time_t time1 = std::chrono::system_clock::to_time_t(date1);
        std::time_t time2 = std::chrono::system_clock::to_time_t(date2);
        std::tm* tm1 = std::localtime(&time1);
        int y1 = tm1->tm_year, m1 = tm1->tm_mon, d1 = tm1->tm_mday;
        std::tm* tm2 = std::localtime(&time2);
        return y1 == tm2->tm_year && m1 == tm2->tm_mon && d1 == tm2->tm_mday;
    }

    /**
     * Check if date1 is on or before date2.
     */
    static bool isOnOrBefore(Date date1, Date date2) {
        return date1 <= date2;
    }

    /**
     * Format date as string (YYYY-MM-DD).
     */
    static std::string formatDate(Date date) {
        std::time_t time = std::chrono::system_clock::to_time_t(date);
        std::tm* tm = std::localtime(&time);
        std::ostringstream oss;
        oss << std::setfill('0') 
            << (tm->tm_year + 1900) << "-"
            << std::setw(2) << (tm->tm_mon + 1) << "-"
            << std::setw(2) << tm->tm_mday;
        return oss.str();
    }

    /**
     * Get day of week name.
     */
    static std::string getDayOfWeekName(Date date) {
        static const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", 
                                      "Thursday", "Friday", "Saturday"};
        return days[getDayOfWeek(date)];
    }

private:
    /**
     * Get number of days in a month.
     */
    static int getDaysInMonth(int year, int month) {
        static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int days = daysInMonth[month - 1];
        // Handle leap year for February
        if (month == 2 && isLeapYear(year)) {
            days = 29;
        }
        return days;
    }

    /**
     * Check if a year is a leap year.
     */
    static bool isLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
};

} // namespace sip

#endif // DATE_UTILS_H
