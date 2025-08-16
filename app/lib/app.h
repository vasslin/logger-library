#pragma once

#include <mutex>
#include <thread>

#include "../../include/loggerlib/file_logger.h"
#include "ts_queue.h"

namespace app {

const LogConfig kLimiter = LogConfig{"", LogLevel::INFO, 0};
const int kSetLogThreadsCnt = 2;
const int kGetLogThreadsCnt = 1;
const std::string kStopWord = "STOP";

class App {
   public:
    App() = default;
    bool execute(const std::string& filename, LogLevel default_level);

   private:
    std::mutex mut_;
    tsQueue<LogConfig> ts_queue_ = tsQueue<LogConfig>(kLimiter);
};

}  // namespace app