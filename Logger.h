#ifndef _Logger_h
#define _Logger_h

#include "./noncopyable.h"

#include <iostream>

enum class LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
    FATAL, // core错误信息
    DEBUG, // 调试
};

#define LOG_INFO(logmsgFormat, ...)                      \
    do                                                   \
    {                                                    \
        Logger &log = Logger::instance();                \
        log.setLogLevel(LogLevel::INFO);                 \
        char buf[1024] = {0};                            \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                    \
    } while (0);

#define LOG_FATAL(logmsgFormat, ...)                     \
    do                                                   \
    {                                                    \
        Logger &log = Logger::instance();                \
        log.setLogLevel(LogLevel::FATAL);                \
        char buf[1024] = {0};                            \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);  \
        exit(-1);                                  \
    } while (0);

#ifdef LOG_DEBUG
#define LOG_DEBUG(logmsgFormat, ...)                     \
    do                                                   \
    {                                                    \
        Logger &log = Logger::instance();                \
        log.setLogLevel(LogLevel::FATAL);                \
        char buf[1024] = {0};                            \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                    \
    } while (0);

#else
#define LOG_DEBUG(logmsgFormat, ...) ;
#endif

#define LOG_ERROR(logmsgFormat, ...)                     \
    do                                                   \
    {                                                    \
        Logger &log = Logger::instance();                \
        log.setLogLevel(LogLevel::ERROR);                \
        char buf[1024] = {0};                            \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        log.log(buf);                                    \
    } while (0);



class Logger:noncopyable
{
private:
    /* data */
    LogLevel loglevel_;
    Logger(){}

public:
    // 获取logger实例
    static Logger &instance();
    // 设置logger等级
    void setLogLevel(LogLevel logLevel);

    // 写日志
    void log(std::string msg);
    ~Logger(){}
};

#endif