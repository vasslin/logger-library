#include "socket_log_server.h"

using namespace socket_server;

SocketLogServer::SocketLogServer(uint32_t ip, uint16_t port) : ip_(ip), port_(port) {
    struct sockaddr_in addr;
    int bytes_read;

    listener_ = socket(AF_INET, SOCK_STREAM, port_);
    if (listener_ < 0) {
        std::cerr << "Error in creating server socket\n";
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = htonl(ip_);

    if (bind(listener_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(listener_);
        return;
    }

    if (listen(listener_, kSizeOfRequestQueue) < 0) {
        std::cerr << "Error in listening server socket\n";
        close(listener_);
        return;
    }
}

bool SocketLogServer::run(time_t time) {
    bool stop = false;

    // for time statistic
    auto now = std::chrono::system_clock::now();
    time_t curr_time = std::chrono::system_clock::to_time_t(now);
    tsCounter<time_t> last_diff_time = curr_time;

    // for count statistic
    tsCounter<size_t> counter = 0;
    std::condition_variable count_condition;

    auto get_func = [this, &stop, &last_diff_time, &counter, &count_condition]() mutable {
        for (;;) {
            {
                std::lock_guard<std::mutex> lk{mut_};
                if (stop) {
                    break;
                }
            }

            // getting message
            char buf[kBuffSize];
            auto sock = accept(listener_, NULL, NULL);
            if (sock < 0) {
                std::cerr << "Error in accepting socket\n";
                return false;
            }
            auto bytes_read = recv(sock, buf, 1024, 0);
            auto log = convertMsgToLogConfig(buf, bytes_read);
            if (log.level != LogLevel::UNDEFINED) {
                stats_coll_.setLog(log);

                // update settings for time/count statistics
                auto now = std::chrono::system_clock::now();
                last_diff_time.set(std::chrono::system_clock::to_time_t(now));
                ++counter;
                if (statistic_interval_cnt_ != 0 && counter.get() % statistic_interval_cnt_ == 0) {
                    count_condition.notify_one();
                }
            }
            close(sock);
        }
        count_condition.notify_all();
        return true;
    };

    auto print_time_func = [this, &stop, &curr_time, &last_diff_time]() mutable {
        // засыпаем на statistic_interval_time_
        // если curr_time - last_mod_time < statistic_interval_time_ --> выводим статистику
        // если stop -> выход из функции;

        time_t time_diff;
        for (;;) {
            {
                std::lock_guard<std::mutex> lk{mut_};
                if (stop) {
                    break;
                }

                auto now = std::chrono::system_clock::now();
                curr_time = std::chrono::system_clock::to_time_t(now);
                time_diff = curr_time - last_diff_time.get();
            }
            if (time_diff < statistic_interval_time_) {
                stats_coll_.printStatistic();
            }
            std::this_thread::sleep_for(std::chrono::seconds(statistic_interval_time_));
        }
    };

    auto print_cnt_func = [this, &stop, &counter, &count_condition]() mutable {
        for (;;) {
            std::unique_lock<std::mutex> lk{mut_};
            count_condition.wait(lk, [this, &stop, &counter]() {
                return stop || (statistic_interval_cnt_ != 0 && counter.get() % statistic_interval_cnt_ == 0);
            });
            if (stop) {
                break;
            }
            lk.unlock();
            stats_coll_.printStatistic();
        }
    };

    std::future<bool> get_sockets = std::async(get_func);
    std::thread print_time_thread(print_time_func);
    std::thread print_cnt_thread(print_cnt_func);

    // server's working time
    std::this_thread::sleep_for(std::chrono::seconds(time));
    {
        std::lock_guard<std::mutex> lk{mut_};
        stop = true;
    }
    print_time_thread.join();
    print_cnt_thread.join();
    return get_sockets.get();
}

LogConfig SocketLogServer::convertMsgToLogConfig(char* buf, size_t sz) {
    std::string msg = std::string(buf);
    std::stringstream ss;
    ss << msg;
    LogConfig log;
    ss >> log.time >> log.level >> log.message;
    return log;
}

uint32_t SocketLogServer::getPort() const { return port_; }
uint16_t SocketLogServer::getIP() const { return ip_; }