#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using std::cout;
using std::endl;


void onConnection(wd::TcpConnectionPtr conn)
{
    cout << conn->toString() << " has connected" << endl;
}
 
void onMessage(wd::TcpConnectionPtr conn)
{
    cout << "before recive msg" << endl;
    //获取消息
    string msg = conn->receive();
    cout << "msg.size():" << msg.size() << endl;
    cout << "msg:" << msg << endl;
    //decode
    //compute
    //encode
    //
    //发送消息
    conn->send(msg);
}

void onClose(wd::TcpConnectionPtr conn)
{
    cout << conn->toString() << " has closed." << endl;
}

int main(void)
{
	//wd::Acceptor acceptor("192.168.30.129", 8000);
	wd::Acceptor acceptor(8000);
	acceptor.ready();

    wd::EventLoop loop(acceptor);
    loop.setAllCallbacks(onConnection, onMessage, onClose);
    loop.loop();

	return 0;
}
