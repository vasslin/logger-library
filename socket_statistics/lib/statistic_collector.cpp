#include "statistic_collector.h"

void StatsCollector::setLog(const LogConfig& log) {
    std::lock_guard<std::mutex> lk{mut_};
    ++important_levels_[log.level];
    times_.push(log.time);
    lengths_.push_back(log.message.size());
}
void StatsCollector::printStatistic() {
    std::lock_guard<std::mutex> lk{mut_};

    auto now = std::chrono::system_clock::now();
    time_t curr_time = std::chrono::system_clock::to_time_t(now);
    while (!times_.empty() && ((curr_time - times_.front()) > times_interval)) {
        times_.pop();
    }

    auto sum = std::accumulate(important_levels_.begin(), important_levels_.end(), size_t(0),
                               [](size_t init, const std::pair<LogLevel, size_t>& pair) { return init + pair.second; });

    std::cout << "-----Statistics-----\n";
    std::cout << "total:\t" << sum << '\n';
    std::cout << "messages by importance level:\n";
    for (int i = static_cast<int>(LogLevel::TRACE); i <= static_cast<int>(LogLevel::FATAL); ++i) {
        auto level = static_cast<LogLevel>(i);
        std::cout << levelToString(level) << " - " << important_levels_[level] << '\n';
    }
    std::cout << "messages from the last hour: " << times_.size() << '\n';

    std::cout << "message length statistics:\n";
    std::cout << "MIN\tMAX\tAVERAGE\n";
    if (!lengths_.empty()) {
        std::cout << *(std::min_element(lengths_.begin(), lengths_.end())) << '\t';
        std::cout << *(std::max_element(lengths_.begin(), lengths_.end())) << '\t';

        auto sum_of_lenghts = std::accumulate(lengths_.begin(), lengths_.end(), size_t(0));
        std::cout << (sum_of_lenghts / lengths_.size()) << '\n';
    } else {
        std::cout << "-1\t-1\t-1\n";
    }
}
