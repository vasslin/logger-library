
#include "lib/app.h"
#include "lib/ts_queue.h"

using namespace app;

int main() {
    std::string file_name = "logs_file.txt";
    LogLevel level = LogLevel::INFO;

    app::App app;
    app.execute(file_name, level);
}