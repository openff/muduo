#include "Logger.h"




// 获取logger实例
Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}
// 设置logger等级
void Logger::setLogLevel(LogLevel logLevel)
{
    this->loglevel_ = logLevel;
}

// 写日志
void Logger::log(std::string msg)
{
    switch (loglevel_)
    {
    case LogLevel::INFO:
        std::cout<<"[INFO]";
        break;
     case LogLevel::ERROR:
        std::cout<<"[ERROR]";
        break;
     case LogLevel::FATAL:
        std::cout<<"[FATAL]";
        break;
     case LogLevel::DEBUG:
        std::cout<<"[DEBUG]";
        break;
    default:
        break;
    }

    std::cout<<" Time:" <<" :"<<msg<<std::endl;
}
