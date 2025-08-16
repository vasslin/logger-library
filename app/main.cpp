
#include "lib/app.h"
#include "lib/ts_queue.h"

using namespace app;

int main() {
    // tsQueue<int> ts_queue{-1};
    // std::cout << 1 << std::endl;

    std::string file_name = "logs_file.txt";
    LogLevel level = LogLevel::INFO;

    app::App app;
    app.execute(file_name, level);
}