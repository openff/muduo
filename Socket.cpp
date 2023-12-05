#include "Socket.h"
#include "InetAddr.h"

#include <unistd.h>
#include <sys/types.h> /* See NOTES */
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <cstring>

#include "Logger.h"
Socket::~Socket()
{
    close(sockfd_);
}

void Socket::bindAddress(const InetAddr &localaddr)
{
    int ret = bind(sockfd_, (sockaddr*)localaddr.getSockAddrInet4(), sizeof(sockaddr_in));
    if (ret < 0)
    {
        LOG_FATAL("bind address error");
    }
}
void Socket::listen()
{
    int ret = ::listen(sockfd_, SOMAXCONN);
    if (ret == -1)
    {
        LOG_FATAL("listen address error");
    }
}
int Socket::accept(InetAddr *peeraddr)
{
    sockaddr_in addr;
    socklen_t len = 0;
    memset(&addr, 0, sizeof addr);
    /**
     * flags：附加选项，可以是 SOCK_NONBLOCK 或 SOCK_CLOEXEC(用于在执行新程序时自动关闭套接字)，用于设置非阻塞或关闭执行标志。
     */
    int connfd = ::accept4(sockfd_, (sockaddr*)&addr, &len, SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        /* 有效连接 */
        peeraddr->setSockAddrInet4(&addr);
    }
    return connfd;
}
void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("shutdown SHUT_WR ERROR");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int enable = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof enable);
}
void Socket::setReuseAddr(bool on)
{
    int enable = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable);
}

void Socket::setReusePort(bool on)
{
    int enable = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof enable);
}

void Socket::setKeepAlive(bool on)
{
    int enable = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof enable);
}