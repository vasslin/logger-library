
#include "../include/loggerlib/file_logger.h"

int main() {
    FileLogger logger("AAA.txt", LogLevel::DEBUG);
    LogConfig log{"some message", LogLevel::WARN};
    logger.writeLog(log);
}