#include "app.h"

LogLevel stringToLevel(const std::string& str) {
    LogLevel log;
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
    return log;
}

bool app::App::execute(const std::string& filename, LogLevel default_level) {
    FileLogger file_logger = FileLogger(filename, default_level);

    if (!file_logger.isValid()) {
        return false;
    }

    // function for writing logs
    auto set_func = [&file_logger, this]() mutable {
        LogConfig log;
        while (ts_queue_.wait_and_pop(log)) {
            file_logger.writeLog(log);
        }
    };

    // function for getting logs from console
    bool ready = false;
    auto get_func = [&file_logger, this, &ready]() mutable {
        std::string message;
        LogLevel level;
        std::stringstream ss;
        std::string str;

        std::unique_lock<std::mutex> ul{mut_};

        while (!ready) {
            std::cin >> str;
            if (str == kStopWord) break;

            ss << str;
            ss >> level;
            message = ss.str();
            ss.str("");
            ss.clear();

            ul.unlock();
            ts_queue_.push(LogConfig{std::move(message), level});
            ul.lock();
        }
        ready = true;
        ul.unlock();
        ts_queue_.wake_and_done();
        return;
    };

    std::cout << "Write logs in format : LEVEL MESSAGE.\nWrite " << kStopWord << " if you finished\n";

    std::vector<std::thread> get_threads(kGetLogThreadsCnt);
    for (auto i = 0; i < kGetLogThreadsCnt; ++i) {
        get_threads[i] = std::thread(get_func);
    }

    std::vector<std::thread> set_threads(2);
    for (auto i = 0; i < 2; ++i) {
        set_threads[i] = std::thread(set_func);
    }

    for (auto& t : get_threads) {
        t.join();
    }
    for (auto& t : set_threads) {
        t.join();
    }
    return true;
}
