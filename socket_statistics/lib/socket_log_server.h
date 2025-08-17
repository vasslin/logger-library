
#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>

#include "../../include/loggerlib/logger_base.h"
#include "statistic_collector.h"
#include "ts_counter.h"

namespace socket_server {

const int kPort = 1234;
const uint16_t kIP = INADDR_ANY;
const size_t kBuffSize = 1024;
const size_t kSizeOfRequestQueue = 10;  // размер очереди запросов для сокета


class SocketLogServer {
   public:
    SocketLogServer(uint32_t ip, uint16_t port);
    ~SocketLogServer() { close(listener_); }

   bool run(time_t time);

    uint32_t getPort() const;
    uint16_t getIP() const;

   private:
    LogConfig convertMsgToLogConfig(char* buf, size_t sz);


    mutable std::mutex mut_;
    uint32_t port_;
    uint16_t ip_;
    int listener_;
    StatsCollector stats_coll_;
    time_t statistic_interval_time_;
    size_t statistic_interval_cnt_;
    
};

}  // namespace socket_server