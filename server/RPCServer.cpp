#include "RPCServer.h"

//--------------------------- CRPCServer -----------------------------
CRPCServer::CRPCServer(int port): CRPCKernel()
{
	m_ListenPort = port;
	m_Listener = -1;
	bzero(&m_Servaddr, sizeof(m_Servaddr));
}

CRPCServer::CRPCServer(int port, int maxEvent): CRPCKernel(maxEvent)
{
	m_ListenPort = port;
	m_Listener = -1;
    bzero(&m_Servaddr, sizeof(m_Servaddr));
}

CRPCServer::~CRPCServer()
{
	//CRPCKernel::~CRPCKernel();
}

int CRPCServer::EpollLoop()
{
	int ret = 0;
	//pthread_mutex_lock(&m_Mutex);
	do
	{
		// start reader threads
		m_Listener = socket(AF_INET, SOCK_STREAM, 0);
		if (-1 == m_Listener)
		{
			ret = 1;
			break;
		}
		SetNonblock(m_Listener);
		m_Servaddr.sin_family = AF_INET;
		m_Servaddr.sin_port = htons(m_ListenPort);
		m_Servaddr.sin_addr.s_addr = INADDR_ANY;
		if (bind(m_Listener, (struct sockaddr *)&m_Servaddr,
				 sizeof(struct sockaddr)) == -1)
		{
			ret = 1;
			break;
		}
		char *listenq = getenv("LISTENQ");
		int listennum = 5;
		if (listenq != NULL)
			listennum = atoi(listenq);
		if (listen(m_Listener, listennum) == -1)
		{
			ret = 1;
			break;
		}
		//if (pipe(m_Pipe) < 0)
			//	printf("pipe failed\n");
		//else
			//	printf("create pipe ok\n");
		if (AddToEpoll(m_Listener, EPOLLIN | EPOLLET))
		{
			if (AddToEpoll(m_Pipe[0], EPOLLIN | EPOLLET))
			{
				printf("add pipe ok\n");
			}
			printf("listen ok\n");
			int count = 0;
			while (true)
			{
				// need to stop
				if (m_Stop == 1)
				{
					printf("stop\n");
					m_Reader->Stop(1);
					m_Writer->Stop(1);
					break;
				}
				int numFD = EpollWait();
				while (HaveNextFD())
				{
					struct epoll_event ev;
					GetNextEvent(&ev);
					struct sockaddr_in clientaddr;
					socklen_t len = sizeof(struct sockaddr_in);
					// there is data in pipe, only when stop we will write
					// data to pipe, 
					if (ev.data.fd == m_Pipe[0])
					{
						printf("pipe wake up\n");
					}
					// epoll listen socket
					else if (ev.data.fd == m_Listener)
					{
						int newfd = accept(m_Listener,(struct sockaddr *) \
										   &clientaddr, &len);
						if (newfd < 0)
							continue;
						else
						{
							printf("connect from %s:%d, socket is %d\n",
								   inet_ntoa(clientaddr.sin_addr),
								   ntohs(clientaddr.sin_port), newfd);
						}
						SetNonblock(newfd);
						if (AddToEpoll(newfd, EPOLLIN | EPOLLET))
						{
							printf("add to epoll\n");
							AddConnection(newfd);
						}
					}// end epoll listen socket
					// epoll connfd
					else
					{
						// add to queue, reader thread will process
						m_Reader->NewReadEvent(ev.data.fd);
					}
				}// while have next fd
			}// while main loop
		}// add listen fd to epoll			
	} while (0);
	//pthread_mutex_unlock(&m_Mutex);
	return ret;	
}
