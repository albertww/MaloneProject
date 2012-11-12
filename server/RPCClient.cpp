#include "RPCClient.h"
//--------------------------- CRPCClient -----------------------------


CRPCClient::CRPCClient(): CRPCKernel()
{
	
}

CRPCClient::CRPCClient(int maxEvents): CRPCKernel(maxEvents)
{
	
}

CRPCClient::~CRPCClient()
{
}

int CRPCClient::EpollLoop()
{
	int ret = 0;
	if (AddToEpoll(m_Pipe[0], EPOLLIN | EPOLLET))
	{
		printf("add pipe ok\n");
	}
	do
	{
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
				// epoll connfd
				else
				{
					// add to queue, reader thread will process
					m_Reader->NewReadEvent(ev.data.fd);
				}
			}// while have next fd
		}// while main loop
	} while (0);
	//pthread_mutex_unlock(&m_Mutex);
	return ret;	
}

int CRPCClient::ConnectTo(string ip_port)
{
	if (GetConnection(ip_port) != NULL)
		return 2;
	int fd;
	do 
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
		{
			printf("socket failed\n");
			return 0;
		}
		struct sockaddr_in servaddr;
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(atoi(GetPort(ip_port).c_str()));
		if (inet_pton(AF_INET, GetIP(ip_port).c_str(), &servaddr.sin_addr) <= 0)
		{
			close(fd);
			printf("inet_pton failed\n");
			return 0;
		}
		if (connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		{
			close(fd);
			printf("connect failed\n");
			return 0;
		}
		
	} while (0);
	// here connect established
	SetNonblock(fd);
	printf("conn established\n");
	if (AddToEpoll(fd, EPOLLIN | EPOLLET))
	{
		printf("add to epoll %i\n", fd);
		AddConnection(fd);
		return 1;
	}
	else
	{
		printf("add to epoll failed %i\n", fd);
		close(fd);
		return 0;
	}
}


void CRPCClient::WriteTo(string ip, string message)
{
	int ret = ConnectTo(ip);
	if (ret > 0)
	{
		printf("prepare to write\n");
		CRPCConnection* pconn = GetConnection(ip);
		CDataPacker packer;
		char buff[1024];
		int val = 0;
		val += packer.SetBuffer(buff, 1024);
		val += packer.WriteBinary(message.c_str(), message.size());
		val += packer.Finish();
		printf("val = %i\n", val);
		printf("packet composed ok\n");
		CRPCPacket *packet = new CRPCPacket(buff, packer.GetPacketLen());
		//pconn->Write(packet);
		m_Writer->WritePacket(ip, packet);
	}
	else
	{
		printf("ret <= 0\n");
	}
}
