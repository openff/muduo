#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include<unistd.h>
#include<sys/syscall.h>
/**
 * __thread 是 C++ 中的一个存储类说明符，用于声明线程局部存储（thread-local storage）的变量。
 * 它指示编译器为每个线程创建一个独立的变量实例，每个线程都有其自己的变量副本，这样每个线程可以独立地访问和修改自己的副本，而不会相互干扰。
 * 
*/
namespace CurrentThread{
    //全局线程id
    extern __thread int t_cachedTid;
    //通过linux API 获取当前系统的thread id通过缓存获取id
    void cacheTid();
    //获取tid
    inline int tid(){
        /**__builtin_expect告诉编译器条件表达式的预期结果，帮助编译器进行优化。*/
        if (__builtin_expect(t_cachedTid =0 , 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}

#endif