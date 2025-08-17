
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/loggerlib/socket_logger.h"
#include "lib/socket_log_server.h"

using namespace socket_server;

int main() {
    SocketLogServer server(INADDR_ANY, 0);
    SocketLogger logger(server.getIP(), server.getPort());

    LogConfig log("some message", LogLevel::WARN);
    if (logger.writeLog(log)) {
        server.run(7);
    }

    StatsCollector stats_coll;
    stats_coll.setLog({"AAAAA", LogLevel::WARN});
    stats_coll.setLog({"AA", LogLevel::INFO});
    stats_coll.setLog({"A", LogLevel::DEBUG});
    stats_coll.setLog({"AAAAAAAA", LogLevel::FATAL});
    stats_coll.printStatistic();
}
