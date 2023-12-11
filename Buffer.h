#ifndef Buffer_H
#define Buffer_H

#include <vector>
#include <algorithm>
#include <cstring>
#include<string>
/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
    explicit Buffer(size_t InitialSize = kInitialSize)
        : buffer_(InitialSize + kInitialSize), readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend)
    {
    }
    ~Buffer();

    size_t readableBytes() const { return writerIndex_ - readerIndex_; }
    size_t writeableBytes() const { return buffer_.size() - writerIndex_; }
    size_t prependableBytes() const { return readerIndex_; }

    // 返回可读首地址
    const char *peek() const { return begin() + readerIndex_; }

    // 复位操作
    void retrieve(size_t len)
    {
        if (len < readableBytes())
        {
            /**只读取了部分数据
             */
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }
    
    // 将buffer 转化成string 返回给用户
    std::string retrieveAsString(size_t len)
    {
        if (len <= readableBytes())
        {
            // 构建
            std::string result(peek(), len);
            // 地址复位
            retrieve(len);
            return result;
        }
        return std::string();
    }

   

    // 可写
    void ensureWriteableBytes(size_t len)
    {
        if (writeableBytes() < len)
        {
            /* 扩容 */
            makeSpace(len);
        }
    }

    //外部拼接api
    void append(const char* data,size_t len){
        ensureWriteableBytes(len);
        std::copy(data,data+len,beginWrite());
        writerIndex_+=len;
    }

    char *begin()
    {
        // buffer_.begin() 迭代器 * 取内容 &返回内容的地址
        return &*buffer_.begin();
    }
    const char *begin() const
    {
        // buffer_.begin() 迭代器 * 取内容 &返回内容的地址
        return &*buffer_.begin();
    }
    char *beginWrite()
    {
        return begin() + writerIndex_;
    }
    const char *beginWrite() const
    {
        return begin() + writerIndex_;
    }


    //从指定fd读取数据
    ssize_t readFd(int fd,int* savaError);
    //从指定fd发送数据
    ssize_t writeFd(int fd,int* savaError);
private:
    // 读数据全部复位
    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
     /* 扩容 */
    void makeSpace(size_t len)
    {
        if (writeableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            /*
             *可写+已读可读大小 < len + 头需要重新申请地址
             */
            buffer_.resize(writerIndex_ + len);
        }
        else
        { // 重用已读内存

            // 获取读内存大小
            size_t readable = readableBytes();
            /**
             * first 和 last：表示输入范围的迭代器区间
                d_first：表示输出范围的迭代器，指向要复制到的目标范围的起始位置。

            */
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }
    /* data */
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

    static const char kCRLF[];
};

#endif