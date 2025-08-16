#pragma once

#include "logger_base.h"

#pragma once

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <iostream>

#include "logger_base.h"

class SocketLogger : public LoggerBase {
   public:
    SocketLogger(uint32_t ip, uint16_t port, LogLevel default_loglevel = LogLevel::INFO);
    virtual ~SocketLogger() noexcept;
    SocketLogger(const SocketLogger& other) = delete;
    SocketLogger operator=(const SocketLogger& other) = delete;

    // FileLogger(FileLogger&& other);
    // FileLogger operator=(FileLogger&& other);


    bool writeLog(const LogConfig& log) override;
    bool isValid() const;

   private:
    std::string createLogString(const LogConfig& log);

    int socket_fd_;
};