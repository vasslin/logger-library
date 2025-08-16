#include "../include/loggerlib/socket_logger.h"

SocketLogger::SocketLogger(uint32_t ip, uint16_t port, LogLevel default_loglevel) {
    int sock;
    struct sockaddr_in addr;

    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        std::cerr << "Error in creating socket.\n";
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ip);
    if (connect(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error in connecting socket.\n";
        socket_fd_ = -1;
        return;
    }
}

SocketLogger::~SocketLogger() noexcept {
    if (socket_fd_ >= 0) {
        close(socket_fd_);
    }
}

// format: time level message
bool SocketLogger::writeLog(const LogConfig& log) {
    std::string message = createLogString(log);
    send(socket_fd_, message.c_str(), sizeof(message.c_str()), 0);
    return true;
}

std::string SocketLogger::createLogString(const LogConfig& log) {
    return std::to_string(log.time) + " " + levelToString(log.level) + " " + log.message;
}

bool SocketLogger::isValid() const { return socket_fd_ >= 0; }
