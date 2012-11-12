

#include "Node.h"
#include "StringUtility.h"

#if 0
void* ThreadFunc(void* param)
{
	printf("server thread start\n");
	CRPCServer *server = (CRPCServer*)param;
	server->Start();
	server->EpollLoop();
}

int main()
{
	CRPCServer server(5000);
	
	//server.EpollLoop();
	pthread_t tid;
	pthread_create(&tid, NULL, ThreadFunc, (void *)&server);
	sleep(10);
	server.Stop(1);
	void* value;
	pthread_join(tid, &value);
}

#endif

int main(int argc)
{
	CNode *node = 0;
	char cmd[1024];
	while (1)
	{
		printf("cmd loop\n");
		scanf("%s", cmd);
		vector<string> params;
		//StrSplitBySpaces(cmd, params);
		StrSplit(cmd, ",", params);
		if (params[0] == "start")
		{
			int port = atoi(params[1].c_str());
			node = new CNode(port, 256);
			node->Start();
		}
		else if (params[0] == "stop")
		{
			if (node)
				node->Stop();
			break;
		}
		else if (params[0] == "send")
		{
			if (node)
				node->SendTo(params[1], params[2]);
		}
	}
	node->WaitJoin();
	if (node)
		delete node;
	return 0;
	
}
