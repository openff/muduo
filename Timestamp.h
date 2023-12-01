#ifndef Timestamp_h
#define Timestamp_h

#include <iostream>
#include<string>
#include <time.h>


class Timestamp
{
private:
    /* data */
    int64_t microSecondsSinceEpoch_;

public:
    //time转化为stirng
    std::string toString() const;
     //返回当前local时间
    static Timestamp now();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    Timestamp(/* args */);
    ~Timestamp();
};



#endif