
#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>

#include "../include/loggerlib/logger_base.h"

namespace socket_server {

const int kPort = 1234;
const size_t kBuffSize = 1024;
const size_t kSizeOfRequestQueue = 10; // размер очереди запросов для сокета

//  сервер, принимающий логи (например, из socket logger'а)
class SocketLogServer {
   public:
    SocketLogServer();

    // связывается с клиентом и берет все его сообщения
    bool getMessagesFromClient(std::vector<LogConfig>& logs);

   private:
   LogConfig convertMsgToLogConfig(char* buf, size_t sz);


   int listener_;

};

}  // namespace socket_server