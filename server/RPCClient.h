/* 
** Author: albert 
** Date: 2012-5-17
** Description: RPC client is used to connect to any RPCServers. A server can have one RPCClient to process
** multiple connections to other RPCServers.
** The RPCClient is similar to RPCServer, it also use epoll to 
*/

#ifndef __RPC_CLIENT_H__
#define __RPC_CLIENT_H__

#include "RPCCommon.h"

#define CLIENT_WRITER_THREAD_NUM 2
#define CLIENT_READER_THREAD_NUM 2

class CRPCClient: public CRPCKernel
{
public:
	CRPCClient();
	CRPCClient(int maxEvent);
	virtual ~CRPCClient();
	virtual int EpollLoop();
	// @return 2 if connection exist, 1 suc, 0 failed
	int ConnectTo(string ip_port);
	void WriteTo(string ip, string message);
};

#endif
