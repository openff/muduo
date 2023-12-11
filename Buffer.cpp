#include "Buffer.h"

#include<error.h>
#include <sys/uio.h>
#include<unistd.h>
/**
 * readv 是一个系统调用函数，用于从文件描述符中读取数据到多个缓冲区中。
 * 它允许一次性从文件描述符读取多个非连续的数据块，提供了一种高效的方式来读取多个缓冲区的数据。
 * 
 * #include <sys/uio.h>
    ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

    struct iovec {
    void *iov_base; // 缓冲区的起始地址
    size_t iov_len; // 缓冲区的长度
};
*/
ssize_t Buffer::readFd(int fd, int *savaError)
{
    char extrabuf[65535];
    iovec vec[1];
    
    const size_t writeable = writeableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writeable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    /**
     * 当这个缓冲区有足够的空间时，不要读入extrabuf。
     * 当使用extrabuf时，我们最多读取128k-1字节。
    */
    const int iovcnt = (writeable < sizeof(extrabuf)) ? 2 : 1;
    const ssize_t n = readv(fd,(const iovec*)&vec,iovcnt);
    if (n < 0)
    {
        *savaError = errno;
    }
    else if (n <= writeable)
    {
        /**未使用vec[1]*/
        writerIndex_+=n;
    }else{
        /*使用vec[1] 拼接*/
        writerIndex_ =buffer_.size();
        append(extrabuf,n - writeable);
    }
    
    return n;

}

ssize_t Buffer::writeFd(int fd,int* savaError){
    ssize_t res = ::write(fd,(void*)writerIndex_,writeableBytes());
    if (res < 0){
        *savaError = errno;
    }
    return res;
}
Buffer::~Buffer()
{
}