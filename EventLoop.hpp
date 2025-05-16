#ifndef __EventLoop_H__
#define __EventLoop_H__

#include "TcpConnection.hpp"
#include <sys/epoll.h>

#include <map>
#include <vector>
using std::vector;
using std::map;

namespace wd
{

class Acceptor;
class EventLoop
{
public:
    EventLoop(Acceptor & a);

    void loop();

    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _onConnection = std::move(cb1);
        _onMessage = std::move(cb2);
        _onClose = std::move(cb3);
    }

private:
    int createEpollFd();
    void addEpollReadEvent(int fd);
    void delEpollReadEvent(int fd);

    void waitEpollFd();//循环执行体
    void handleNewConnection();
    void handleMessage(int);

private:
    int _epfd;
    bool _isLooping;
    Acceptor & _acceptor;
    vector<struct epoll_event> _eventArr;

    map<int, TcpConnectionPtr> _conns;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

}// end of namespace wd


#endif

