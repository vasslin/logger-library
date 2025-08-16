#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <thread>

enum class LogLevel : int { UNDEFINED, TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

std::istream& operator>>(std::istream& in, LogLevel& log);

struct LogConfig {
    LogConfig() = default;
    LogConfig(std::string&& message, LogLevel level);
    LogConfig(const std::string& message, LogLevel level);
    LogConfig(std::string&& message, LogLevel level, time_t time);
    LogConfig(const std::string& message, LogLevel level, time_t time);

    bool operator==(const LogConfig& other);

    time_t time = 0;
    std::string message = "";
    LogLevel level = LogLevel::INFO;
};

class LoggerBase {
   public:
    LoggerBase(const std::string& file_name, LogLevel default_loglevel);
    virtual ~LoggerBase() = default;  // ??

    virtual bool writeLog(const LogConfig& log) = 0;
    void setDefaultLevel(LogLevel level);

   protected:
    std::string levelToString(LogLevel level);

    mutable std::mutex mut_;
    LogLevel default_loglevel_;
    std::string file_name_;
};