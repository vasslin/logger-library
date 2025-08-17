#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

#include "logger_base.h"

class FileLogger : public LoggerBase {
   public:
    FileLogger(const std::string& file_name, LogLevel default_loglevel = LogLevel::INFO);
    FileLogger(const FileLogger& other) = delete;
    FileLogger operator=(const FileLogger& other) = delete;

    ~FileLogger() = default;

    bool writeLog(const LogConfig& log) override;
    bool isValid() const override;

   private:
    std::string createLogString(const std::string& message, LogLevel loglevel, time_t time);

    mutable std::mutex mut_;
    std::string file_name_;
    std::ofstream file_stream_;
};