
#ifndef __NODE_H__
#define __NODE_H__

#include "RPCServer.h"
#include "RPCClient.h"

class CNode
{
public:
	static void* ServerThreadFunc(void* param);
	static void* ClientThreadFunc(void* param);

public:
	CNode(int port, int maxevent);
	~CNode();
	void Start();
	void Stop();
	void SendTo(string ipaddr, string message);
	void WaitJoin();

protected:
	CRPCServer m_Server;
	CRPCClient m_Client;
	pthread_t m_Servertid;
	pthread_t m_Clienttid;
};

#endif