#include "Timestamp.h"

Timestamp::Timestamp(/* args */):microSecondsSinceEpoch_(0)
{
}

Timestamp::~Timestamp()
{

}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch) :microSecondsSinceEpoch_(microSecondsSinceEpoch)
{

}

std::string Timestamp::toString() const {
    char buf[128]={0};
    //将mic时间转化为tm
    tm * temp = localtime(&microSecondsSinceEpoch_);
    snprintf(buf,128,"%4d/%2d/%2d  %2d:%2d:%2d",
    temp->tm_year+1900,
    temp->tm_mon,
    temp->tm_mday,
    temp->tm_hour,
    temp->tm_min,
    temp->tm_sec
    );
    return buf;
}

Timestamp Timestamp::now() {
    //返回当前local时间
    
    return Timestamp(time(NULL));
}


