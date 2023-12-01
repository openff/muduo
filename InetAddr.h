#ifndef INETADDR_H
#define INETADDR_H
#include<netinet/in.h>
#include<string>
class InetAddr
{
private:
    /* data */
    sockaddr_in addr_;
public:
    explicit InetAddr(uint16_t port = 0,std::string ip="127.0.0.1");
    explicit InetAddr(sockaddr_in* addr):addr_(*addr){}
    std::string toIp();
    std::string toIpPort();
    uint16_t toPort();
    ~InetAddr(){}
};




#endif