#ifndef CallBacks_H
#define CallBacks_H

#include"Timestamp.h"


#include<memory>
#include<functional>



class TcpConnection;
class Buffer;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)>;
using HighWriterMarkCallback = std::function<void(const TcpConnectionPtr&,size_t)>;


#endif
