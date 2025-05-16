#ifndef __WD_ACCEPTOR_H__
#define __WD_ACCEPTOR_H__

#include "InetAddress.hpp"
#include "Socket.hpp"

namespace wd
{

class Acceptor
{
public:
	Acceptor(unsigned short port);
	Acceptor(const string & ip, unsigned short port);
	void ready();

    int fd() const {    return _listensock.fd();    }

	int accept();
private:
	void setReuseAddr(bool on);
	void setReusePort(bool on);
	void bind();
	void listen();
private:
	InetAddress _addr;
	Socket _listensock;
};

}//end of namespace wd

#endif
