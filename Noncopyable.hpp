#ifndef __WD_Noncopyable_H__
#define __WD_Noncopyable_H__

class Noncopyable
{
protected:
    Noncopyable() {}
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable& operator=(const Noncopyable &) = delete;
};


#endif

