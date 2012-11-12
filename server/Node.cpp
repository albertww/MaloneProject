#include "Node.h"

void* CNode::ServerThreadFunc(void* param)
{
	printf("server thread start\n");
	CRPCServer *server = (CRPCServer*)param;
	server->Start();
	server->EpollLoop();
}

void* CNode::ClientThreadFunc(void* param)
{
	printf("client thread start\n");
	CRPCClient *client = (CRPCClient*)param;
	client->Start();
	client->EpollLoop();
}

CNode::CNode(int port, int maxevent): m_Server(port, maxevent), m_Client(port)
{
}

CNode::~CNode()
{
}

void CNode::Start()
{
	//m_Server.Start();
	//m_Client.Start();
	pthread_create(&m_Servertid, NULL, ServerThreadFunc, (void *)&m_Server);
	pthread_create(&m_Clienttid, NULL, ClientThreadFunc, (void *)&m_Client);
}

void CNode::Stop()
{
	m_Server.Stop(1);
	m_Client.Stop(1);
	printf("client stop\n");
}

void CNode::SendTo(string ipaddr, string message)
{
	printf("node send to %s\n", ipaddr.c_str());
	m_Client.WriteTo(ipaddr, message);
}

void CNode::WaitJoin()
{
	void* value;
	pthread_join(m_Clienttid, &value);
	pthread_join(m_Servertid, &value);
}

