#include "logger.h"

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

std::string Logger::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return COLOR_WHITE;
        case LogLevel::WARNING: return COLOR_YELLOW;
        case LogLevel::ERROR:   return COLOR_RED;
        default:                return COLOR_RESET;
    }
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::string color = getColorCode(level);
    std::string levelStr = getLevelString(level);
    std::string timestamp = getCurrentTimestamp();

    std::cout << color
              << "[" << levelStr << "] "
              << "[" << timestamp << "] : "
              << message
              << COLOR_RESET
              << std::endl;
}
