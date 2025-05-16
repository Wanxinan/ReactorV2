#include "EventLoop.hpp"
#include "Acceptor.hpp"
#include "TcpConnection.hpp"
#include <errno.h>
#include <stdio.h>
#include <string.h>


namespace wd
{

EventLoop::EventLoop(Acceptor & a)
: _epfd(createEpollFd())
, _acceptor(a)
, _isLooping(false)
, _eventArr(100)
{
    addEpollReadEvent(_acceptor.fd());
}


void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping) {
        waitEpollFd();
    }
}

void EventLoop::waitEpollFd()
{
    int nready = epoll_wait(_epfd, _eventArr.data(), _eventArr.size(), -1);
    if(nready == -1 && errno == EINTR) {
        //是软中断的错误
        return;
    } else if(nready == -1) {
        perror("epoll_wait");
        return ;
    } else if(nready == 0) {
        printf("epoll timeout.\n");
    } else {
        //nready > 0
        for(int i = 0; i < nready; ++i) {
            int fd = _eventArr[i].data.fd;
            if(fd == _acceptor.fd()) {
                //新连接的处理
                handleNewConnection();
            } else {
                //已经建立好的连接
                handleMessage(fd);
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int connfd = _acceptor.accept();
    //connfd添加到epoll的红黑树
    addEpollReadEvent(connfd);

    TcpConnectionPtr sp(new TcpConnection(connfd));
    _conns.insert(std::make_pair(connfd, sp));
    
    sp->setAllCallbacks(std::move(_onConnection), 
                        std::move(_onMessage), 
                        std::move(_onClose));

    //当连接建立的时候，要调用函数对象
    sp->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    //消息到达时
    //先通过fd 找到TcpConnection对象
    auto iter = _conns.find(fd);
    if(iter != _conns.end()) {
        //先判断连接是否断开
        ////iter->second就是TcpConnectionPtr
        bool isClosed = iter->second->isClosed();
        if(isClosed) {
            //连接断开
            iter->second->handleCloseCallback();
            //从红黑树上要删除掉
            delEpollReadEvent(fd);
            _conns.erase(fd);//从map删除掉记录
        } else {
            //消息处理
            iter->second->handleMessageCallback();
        }
    }
}

int EventLoop::createEpollFd()
{
    int fd = epoll_create1(0);
    if(fd < 0) {
        perror("epoll_create1");
    }
    return fd;
}



void EventLoop::addEpollReadEvent(int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;//监听读事件
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret < 0) {
        perror("epoll_ctl");
    }
}

void EventLoop::delEpollReadEvent(int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &event);
    if(ret < 0) {
        perror("epoll_ctl");
    }
}



}//end of namespace wd
