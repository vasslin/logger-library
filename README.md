# logger-library

## A thread-safe logging system.
The project consists of three parts:
- logger library
- multithreaded console app for checking the library of logging messages
- a console program for collecting statistics based on socket data (from socket logger library)


## Structure of project

```
log_system/
├── include/loggerlib      # header files
│   ├── logger.h           # base logger interface
│   ├── file_logger.h      # logger to file
│   └── socket_logger.h    # logger to socket
├── src/   
│   ├── logger.cpp           
│   ├── file_logger.cpp      
│   └── socket_logger.cpp 
├── app/               
│   ├── CMakeLists.txt   
│   ├── main.cpp            # app usage examples 
│   └── lib 
│       ├── app.h           # ts app for checking file logger
│       ├── app.cpp        
│       ├── ts_queue.h      # thread-safe queue
│       └── ts_queue.cpp 
├── socket_statistics/             
│   ├── CMakeLists.txt   
│   ├── main.cpp
│   └── lib
│       ├── ts_counter.h
│       ├── socket_log_server.h    # server for receiving logs from a socket
│       ├── socket_log_server.cpp        
│       ├── statistic_collector.h   # class for printing statistic
│       └──  statistic_collector.cpp 
├── tests/      # unit-tests           
└── CMakeLists.txt         
```

# logger library


All logs are represented as objects of the _LogConfig_ class,  which has the fields:
- `time` (time of log creation)
- `message` (log message)
- `level` (levels of importance)

The levels of importance are represented as an enum class and have 6 levels of difficulty: __TRACE, DEBUG, INFO, WARN, ERROR, FATAL__.

There is also an __UNDEFINED__ service level.

It is possible to set default LogLevel `default_loglevel_`. Logs with level less than `default_loglevel_` will be skipped.

Logger library consists of two parts:

1. File Logger _FileLogger_
2. Socket logger _SocketLogger_

All classes have the same interface from base class _LoggerBase_:
- `bool writeLog(const LogConfig& log)` - writes Log log to journal. If successful, it returns the value true. Otherwise, returns false.
- `bool isValid()` - checks if logger is correct


### File logger
FileLogger for writing text messages to file.

Main functions of class FileLogger:
- `Constructor` - creates FileLogger with file_name (journal name) and default log level parameters.
- `createLogString(const string& message, LogLevel loglevel, time_t time)` - creates string in format `[Y-m-d H:M:S] [LEVEL] MESSAGE`


Class is thread-safe. FileLogger has mutex to protect shared data (file stream and output stream) from being simultaneously accessed by multiple threads.

### Socket logger

SocketLogger is a thread-safe class for sending structured log messages to a remote server over a TCP socket. It is an implementation of the _LoggerBase_ interface and provides:

- `Constructor` - creates SocketLogger with ip, port and default log level paremeters. Сreates a socket, configures the server address, and establishes a connection to the server.
- `createLogString(const string& message, LogLevel loglevel, time_t time)` - creates string in format `TIME LEVEL MESSAGE` (TIME - time in time_t format)
- `Destructor` - in the destructor, the socket is closed and the connection to the server is disconnected, respectively (if the socket is active).



# Console app for checking FileLogger.

The application consists of class `App` which is defined in `app` namespace. It accepts logs from the std::cin input stream in the __LEVEL MESSAGE__ format and writes them to a text log.

Main functions:
- `bool execute(const string& filename, LogLevel default_level)` - executes app's work. If successful, it returns the value true. Otherwise, returns false.

In the `execute` method there was two main functions: 
- `get_func()` - function gets log message from the input stream and pushes it to thread-safe queue with logs.
- `set_func()` - function writes logs from thread-safe queue to the FileLogger.


App uses a thread-safe queue `ts_queue_` (which is implemented in the ts_queue.h and ts_queue.inl files). This queue contains logs from console for writing to the journal.


The number of threads with main functions `get_func()` and `set_func()` are regulated by constants _kGetLogThreadsCnt_ and _kSetLogThreadsCnt_ respectively (in namespace app).

When user writes `kStopWord` (which has default value "STOP"), the threads finish their work.


An example of usage will result in the [main.cpp](app/main.cpp):

```
#include "lib/app.h"
#include "lib/ts_queue.h"

using namespace app;

int main() {
    std::string file_name = "logs_file.txt";
    LogLevel level = LogLevel::INFO;

    app::App app;
    app.execute(file_name, level);
}
```


### Thread-Safe Queue

App uses a template thread-safe queue. 

`tsQueue` is a template class for a thread-safe queue that implements:
- Safely add/extract items from multiple streams
- Mechanism for blocking the consumer thread when the queue is empty
- Shutdown alarm via a special limiter element

To regulate synchronization between threads, the queue uses __mutex__ and __condition_variable__.

Class members:
- `std::queue<T> q_` standart queue
- `mutable std::mutex mut_` - mutex for synchronization
- `std::condition_variable cond_var` condition variable  for regulating the push-pop operations.
- `T limiter_` - a marker to indicate the end of the queue


Interface of the ts-queue:

- `void push(T value)` - pushes value to the queue. It locks mutex with using std::lock_guard<std::mutex> and then pushes value to the queue. Thread notifies one of the threads which are waitng for elements from method `wait_and_pop`
- `bool wait_and_pop(T& data)` - pops element from queue. It is blocked while the queue is empty. Returns false (stop signal) when receiving a special element Limiter. In all other cases, moves the element to data and returns true.
- `void wake_and_done()` - called when there are no more new elements  in the queue. notify_all() wakes up all waiting threads.






# Console program for collecting statistics based on socket data

A server has been implemented to collect and analyze logs transmitted over TCP sockets. The server provides real-time statistics with configurable output intervals.

### Architecture

Componemts:

- `SocketLogServer` - server class
- `StatsCollector` - class for counting statistics

1. _SocketLogServer_

In the constructor, a TCP socket is created, bound to the specified IP/port, and listening begins.

__Error handling:__
- Error logging bind/connect
- Closing sockets correctly in case of errors


The main work cycle
`bool SocketLogServer::run(time_t time)` :
Starts three threads:
1. Receiving messages (`get_func`)
- Accepts connections
- Reads data
- Updates statistics

2. Timer output (`print_time_func`)
- Periodic output (statistic_interval_time_ interval)

3. Counter output (`print_cnt_func`)
- Output every N messages (statistic_interval_cnt_)


2. _StatsCollector_

Collects:
- Number of messages by level (in unordered_map<LogLevel, size_t> important_levels_)
- Message lengths (in vector<size_t> lengths_)
- Messages for the last hour (timestamp analysis) (in queue<time_t> times_)

Example of statistics:

```
-----Statistics-----
total:  4
messages by importance level:
Trace - 0
Debug - 1
Info - 1
Warn - 1
Error - 0
Fatal - 1
messages from the last hour: 4
message length statistics:
MIN     MAX     AVERAGE
1       8       4
```

The programm is thread-safe.
`SocketLogServer` uses tsCounter (thread-safe counter), which is defined in [ts_counter.h](socket_statistics/lib/ts_counter.h)