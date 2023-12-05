#include "InetAddr.h"

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
InetAddr::InetAddr(uint16_t port, std::string ip)
{
    std::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    /**
     * ip:
     *  * 本地-> 网络 inet_addr
     *  * 反之        inet_ntoa
     * port:
     *  * 本地-> 网络 htons  ip4     htonl ip6
     *  * 反之         ntohs ip4     ntohl ip6
     */
    addr_.sin_addr.s_addr = inet_addr(ip.data());
    addr_.sin_port = htons(port);
}
/**
 * 上面接收是本地->网络  
 * 下面是 网络 -> 本地
*/
std::string InetAddr::toIp()
{
    std::string ip{inet_ntoa(addr_.sin_addr)};
    return ip;
}

uint16_t InetAddr::toPort()
{
    return ntohs(addr_.sin_port);
}
std::string InetAddr::toIpPort()
{
    std::string ipPort(toIp());
    ipPort += ":" + std::to_string(toPort());
    return ipPort;
}
void InetAddr::setSockAddrInet4(sockaddr_in *addr){
    addr_=*addr;
}