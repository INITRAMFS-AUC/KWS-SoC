#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <memory>
#include <cstdio>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // Log methods with formatting support
    template<typename... Args>
    static void info(const std::string& format, Args... args) {
        log(LogLevel::INFO, formatString(format, args...));
    }

    template<typename... Args>
    static void warning(const std::string& format, Args... args) {
        log(LogLevel::WARNING, formatString(format, args...));
    }

    template<typename... Args>
    static void error(const std::string& format, Args... args) {
        log(LogLevel::ERROR, formatString(format, args...));
    }

    // Overloads for simple string messages (no formatting)
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    // ANSI color codes
    static constexpr const char* COLOR_RESET = "\033[0m";
    static constexpr const char* COLOR_WHITE = "\033[37m";
    static constexpr const char* COLOR_YELLOW = "\033[33m";
    static constexpr const char* COLOR_RED = "\033[31m";

    // Helper methods
    static std::string getCurrentTimestamp();
    static std::string getColorCode(LogLevel level);
    static std::string getLevelString(LogLevel level);
    static void log(LogLevel level, const std::string& message);

    // Formatting helper
    template<typename... Args>
    static std::string formatString(const std::string& format, Args... args) {
        int size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
        if (size <= 0) {
            return format;
        }
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }
};

#endif // LOGGER_H
