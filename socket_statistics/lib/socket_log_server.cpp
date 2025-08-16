#include "socket_log_server.h"

using namespace socket_server;

SocketLogServer::SocketLogServer() {
    struct sockaddr_in addr;

    listener_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_ < 0) {
        std::cerr << "Error in creating server socket\n";
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(kPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error in binding server socket\n";
        return;
    }
}

// функция связывается с одним клиентом и сохраняет все переданные в него логи в вектор logs
bool SocketLogServer::getMessagesFromClient(std::vector<LogConfig>& logs) {
    char buf[kBuffSize];
    int sock;
    listen(listener_, kSizeOfRequestQueue);

    sock = accept(listener_, NULL, NULL);
    while (sock >= 0) {
        auto sz = recv(sock, buf, 1024, 0);
        logs.push_back(convertMsgToLogConfig(buf, sz));
        close(sock);
        sock = accept(listener_, NULL, NULL);
    }
    return true;
}

LogConfig SocketLogServer::convertMsgToLogConfig(char* buf, size_t sz) {
    std::string msg = std::string(buf);
    std::stringstream ss;
    ss << msg;
    LogConfig log;
    ss >> log.time >> log.level >> log.message;
    return log;
}
