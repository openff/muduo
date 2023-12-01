#include"CurrentThread.h"

 namespace CurrentThread
 {
    __thread int t_cachedTid = 0;
    //通过linux API 获取当前系统的thread id
    void cacheTid(){
        if (t_cachedTid == 0){
            /**
             * syscall 函数用于在 C/C++ 程序中调用底层的系统调用
             * 调用 SYS_gettid 来获取当前线程的线程 ID，并将其转换为 pid_t 类型后赋值给变量 t_cachedTid。
            */
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
 } // namespace CurrentThread
