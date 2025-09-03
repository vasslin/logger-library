#include "../include/loggerlib/logger_base.h"

LogConfig::LogConfig(std::string&& message, LogLevel level) : message(std::move(message)), level(level) {
    auto now = std::chrono::system_clock::now();
    time = std::chrono::system_clock::to_time_t(now);
}

LogConfig::LogConfig(const std::string& message, LogLevel level) : message(message), level(level) {
    auto now = std::chrono::system_clock::now();
    time = std::chrono::system_clock::to_time_t(now);
}

LogConfig::LogConfig(std::string&& message, LogLevel level, time_t time)
    : message(std::move(message)), level(level), time(time) {}

LogConfig::LogConfig(const std::string& message, LogLevel level, time_t time)
    : message(message), level(level), time(time) {}

bool LogConfig::operator==(const LogConfig& other) {
    return (level == other.level) && (time == other.time) && (message == other.message);
}

std::istream& operator>>(std::istream& in, LogLevel& log) {
    std::string str;
    in >> str;

    if (str == "TRACE") {
        log = LogLevel::TRACE;
    } else if (str == "DEBUG") {
        log = LogLevel::DEBUG;
    } else if (str == "INFO") {
        log = LogLevel::INFO;
    } else if (str == "WARN") {
        log = LogLevel::WARN;
    } else if (str == "ERROR") {
        log = LogLevel::ERROR;
    } else if (str == "FATAL") {
        log = LogLevel::FATAL;
    } else {
        log = LogLevel::UNDEFINED;
    }
    return in;
}

LoggerBase::LoggerBase(LogLevel level) : default_loglevel_(level) {}

std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:
            return std::string{"Trace"};
        case LogLevel::DEBUG:
            return std::string{"Debug"};
        case LogLevel::INFO:
            return std::string{"Info"};
        case LogLevel::WARN:
            return std::string{"Warn"};
        case LogLevel::ERROR:
            return std::string{"Error"};
        case LogLevel::FATAL:
            return std::string{"Fatal"};

        default:
            return std::string{"Undefined"};
    }
}

void LoggerBase::setDefaultLevel(LogLevel level) { default_loglevel_ = level; }

LoggerBase::~LoggerBase() {}
