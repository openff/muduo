#ifndef  _NONCOPYABLE_H
#define  _NONCOPYABLE_H

/**
 * 关闭noncopyable 派生类的copy操作
*/
class  noncopyable
{
public:

    noncopyable(const noncopyable&)=delete;
    noncopyable& operator =(const noncopyable&)=delete;
protected:
     noncopyable(/* args */)=default;
    ~ noncopyable()=default;
};




#endif