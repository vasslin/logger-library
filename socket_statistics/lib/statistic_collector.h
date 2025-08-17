
#include "../../include/loggerlib/socket_logger.h"
#include <unordered_map>
#include <queue>
#include <mutex>
#include <numeric>
#include <algorithm>


class StatsCollector {
   public:
    void setLog(const LogConfig& log);
    void printStatistic();

   private:
   std::mutex mut_;

   // statistics on the number of messages
   std::unordered_map<LogLevel, size_t> important_levels_;
   std::queue<time_t> times_;
   const time_t times_interval = 60 * 60; // 1 hour

   // length of messages
   std::vector<size_t> lengths_;
};