/* 
** Author: albert 
** Date: 2012-4-21
** Description: RPC server, listened on epoll, and have multiple connections, each connection
** associate with a RPC client
*/
#ifndef __RPC_SERVER_H__
#define __RPC_SERVER_H__

#include "RPCCommon.h"

class CRPCServer: public CRPCKernel
{
public:
	CRPCServer(int port);
	CRPCServer(int port, int maxEvent);
	virtual ~CRPCServer();
	// @overrid CEpoll
	virtual int EpollLoop();
protected:
	int m_ListenPort;
	int m_Listener;
	struct sockaddr_in m_Servaddr;
};

#endif

