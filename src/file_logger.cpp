#include "../include/loggerlib/file_logger.h"

FileLogger::FileLogger(const std::string& file_name, LogLevel default_loglevel)
    : LoggerBase(default_loglevel), file_name_(file_name) {
    file_stream_ = std::ofstream(file_name_, std::ios::app);
}

std::string FileLogger::createLogString(const std::string& message, LogLevel loglevel, time_t time) {
    std::tm curr_time{};

#ifdef _WIN32
    localtime_s(&curr_time, &time);
#else
    localtime_r(&time, &curr_time);
#endif

    std::stringstream ss;

    ss << "[" << std::put_time(&curr_time, "%Y-%m-%d %H:%M:%S") << "] "
       << "[" << levelToString(loglevel) << "] " << message;

    return ss.str();
}

bool FileLogger::writeLog(const LogConfig& log) {
    if (!file_stream_.is_open()) {
        std::lock_guard<std::mutex> lk{mut_};
        std::cerr << "Unable to open a file\n";
        return false;
    }
    if (static_cast<int>(log.level) > static_cast<int>(LogLevel::FATAL)) {
        std::lock_guard<std::mutex> lk{mut_};
        std::cerr << "Invalid log level\n";
        return false;
    }
    if (static_cast<int>(log.level) < static_cast<int>(default_loglevel_)) {
        return true;
    }

    auto log_text = createLogString(log.message, log.level, log.time);

    {
        std::lock_guard<std::mutex> lk{mut_};
        file_stream_ << log_text << '\n';
        file_stream_.flush();
    }
    return true;
}

bool FileLogger::isValid() const {
    std::lock_guard<std::mutex> lk{mut_};
    if (!file_stream_.is_open()) {
        std::cerr << "Unable to open a file\n";
        return false;
    }
    return true;
}
