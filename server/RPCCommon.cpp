#include "RPCCommon.h"

// -------------------------- CThreadSafeQueue -------------------------

template <class T>
CThreadSafeQueue<T>::CThreadSafeQueue()
{
	pthread_cond_init(&m_Cond, NULL);
	pthread_mutex_init(&m_Mutex, NULL);
}

template <class T>
CThreadSafeQueue<T>::~CThreadSafeQueue()
{
	pthread_cond_destroy(&m_Cond);
	pthread_mutex_destroy(&m_Mutex);
}

template <class T>
int CThreadSafeQueue<T>::Push(const T &data)
{
	pthread_mutex_lock(&m_Mutex);	
	m_Queue.push_back(data);
	pthread_mutex_unlock(&m_Mutex);
	pthread_cond_signal(&m_Cond);
	return 1;
}

template <class T>
int CThreadSafeQueue<T>::Pop(T &data)
{
	pthread_mutex_lock(&m_Mutex);
	while (m_Queue.size() == 0)
	{
		pthread_cond_wait(&m_Cond, &m_Mutex);
	}
	data = *(m_Queue.begin());
	m_Queue.pop_front();
	pthread_mutex_unlock(&m_Mutex);
	return 1;
}

template <class T>
int CThreadSafeQueue<T>::Size()
{
	return m_Queue.size();
}

// -------------------------- CRPCReadFDQueue -------------------------

CRPCReadFDQueue::CRPCReadFDQueue()
{
}

CRPCReadFDQueue::~CRPCReadFDQueue()
{
}

int CRPCReadFDQueue::Push(const string &data)
{
	if (data == "")
		return 0;
	return CThreadSafeQueue<string>::Push(data);
}

int CRPCReadFDQueue::Pop(string &data)
{
	return CThreadSafeQueue<string>::Pop(data);
}

void CRPCReadFDQueue::Wakeup(int num)
{
	for (int idx = 0; idx < num; idx ++)
	{
		pthread_mutex_lock(&m_Mutex);
		m_Queue.push_back("");
		pthread_mutex_unlock(&m_Mutex);
		pthread_cond_signal(&m_Cond);
	}
}
// -------------------------- CConnectionReadBuffer -------------------------
CConnectionReadBuffer::CConnectionReadBuffer()
{
	Reset();
}

void CConnectionReadBuffer::Reset()
{
	m_Cursor = 0;
	m_Len = 0;
}

void CConnectionReadBuffer::Adjust()
{
	if (m_Len == m_Cursor)
	{
		m_Cursor = 0;
		m_Len = 0;
	}
	else if (m_Len > m_Cursor && m_Cursor != 0)
	{
		memcpy(m_Buff, m_Buff + m_Cursor, m_Len - m_Cursor);
		m_Len = m_Len - m_Cursor;
		m_Cursor = 0;
	}
}
int CConnectionReadBuffer::Remainder()
{
	return BUFF_LEN - m_Len;
}

int CConnectionReadBuffer::GetMaxBuffLen()
{
	return BUFF_LEN;
}

char * CConnectionReadBuffer::GetWholeBuffer()
{
	return m_Buff;
}

int CConnectionReadBuffer::Read(int fd, list<CRPCPacket *> &li)
{
	int ret = -1;
	while (1)
	{
		if (Remainder() > 0)
		{
			int len = read(fd, m_Buff + m_Len, Remainder());
			// data read
			if (len > 0)
			{
				printf("read %i datas\n", len);
				m_Len += len;
				while (1)
				{
					CRPCPacket *packet = GetPacket();
					if (packet)
					{
						m_Cursor += packet->GetPacketLen();
						li.push_back(packet);
					}
					else
					{
						Adjust();
						break;
					}
				}
			}
			// maybe error or no more data can be read
			else if (len < 0)
			{
				if (errno == EAGAIN)
				{
					printf("eagain\n");
					ret = 1;
					break;
				}
				else
				{
					ret = -1;
					break;
				}
			}
			// read end, closed
			else
			{
				printf("CConnectionReadBuffer read end\n");
				ret = 0;
				break;
			}
		}
		else
		{
			Reset();
		}
	}
	return ret;
}


CRPCPacket * CConnectionReadBuffer::GetPacket()
{
	// at least we have received enough data for a header
	if (m_Len - m_Cursor >= RPC_PACKET_HEADER_LEN)
	{
		CDataUnPacker unpacker;
		// there is a packet header and it is valid
		int val = unpacker.SetBuffer(m_Buff + m_Cursor, m_Len - m_Cursor);
		// buffer contain a packet
		if (val == 1)
		{
			// there is an integrated packet
			if (unpacker.IsValid())
			{
				CRPCPacket *packet = new CRPCPacket(m_Buff + m_Cursor, 
					unpacker.GetPacketLen());
				//m_Cursor += unpacker.GetPacketLen();
				//memcpy(packet->GetBuffer(), m_Buff + m_Cursor, unpacker.GetPacketLen());
				return packet;
			}
			else
			{
				printf("unpacker error: setbuffer suc but is not valid, impossible\n");
				Reset();
			}
		}
		// buffer is not long enough to contain a packet
		else if (val == 0)
		{
			return NULL;
		}
		// buffer data is not valid to compose a packet
		else if (val == -1)
		{
			printf("un-recognized read buffer\n");
			Reset();
			return NULL;
		}
		// not possible
		else
		{
			printf("not possible\n");
			return NULL;
		}
	}
	// there is not sufficient data for a header
	else
		return NULL;
}

// -------------------------- CRPCConnection -------------------------
CRPCConnection::CRPCConnection(int fd): m_SocketFD(fd), m_IsValid(0),
										m_IsClosed(0), m_IsSending(0)
{
	bzero(&m_PeerAddr, sizeof(m_PeerAddr));
	socklen_t len = sizeof(m_PeerAddr);
	if (0 == getpeername(fd, (struct sockaddr *)&m_PeerAddr, &len))
	{
		m_IPAddress = SockaddrToString(&m_PeerAddr);
		if (m_IPAddress == "")
			m_IsValid = 0;
		else
			m_IsValid = 1;
	}
	m_LocalIPAddress = SockFDToLocalString(m_SocketFD);
}

CRPCConnection::~CRPCConnection()
{
}

string CRPCConnection::GetIP()
{
	int i = m_IPAddress.find(':');
	if (-1 == i)
		return "";
	return m_IPAddress.substr(0, i);
}

string CRPCConnection::GetPort()
{
	int i = m_IPAddress.find(':');
	if (-1 == i)
		return "";
	return m_IPAddress.substr(i + 1);
}

string CRPCConnection::GetIPAddress()
{
	return m_IPAddress;
}

string CRPCConnection::GetLocalAddress()
{
	return m_LocalIPAddress;
}

int CRPCConnection::GetSocketFD()
{
	return m_SocketFD;
}

int CRPCConnection::IsValid()
{
	return m_IsValid;
}

int CRPCConnection::GetDirection()
{
	return m_Direction;
}

int CRPCConnection::Read()
{
	if (m_IsValid == 0)
		return -1;
	list<CRPCPacket *> li;
	printf("conn read packet\n");
	int ret = m_PacketBuffer.Read(m_SocketFD, li);
	if (ret == 1)
	{
		
	}
	else if (ret == 0)
	{
		printf("%s read end\n", m_IPAddress.c_str());
		//close(m_SocketFD);
	}
	if (ret < 0)
	{
		printf("%s read error\n", m_IPAddress.c_str());
		//close(m_SocketFD);
	}
	while (1)
	{
		list<CRPCPacket *>::iterator it = li.begin();
		if (it != m_InPackets.end())
		{
			break;
		}
		else
		{
			m_InPackets.push_back(*it);
			li.pop_front();
		}
	}
	return ret;
}

int CRPCConnection::Write(CRPCPacket *packet)
{
	if (m_IsValid == 0)
		return -1;
	m_OutPackets.Push(packet);
	return 1;
}

int CRPCConnection::IsSending()
{
	return m_IsSending;
}

void CRPCConnection::SetSending(int flag)
{
	m_IsSending = flag;
}

int CRPCConnection::GetNumOfOutPackets()
{
	return m_OutPackets.Size();
}

int CRPCConnection::Send()
{
	if (m_OutPackets.Size() < 1 || m_IsValid == 0)
	{
		return 0;
	}
	CRPCPacket* pack;
	m_OutPackets.Pop(pack);
	int count = 0;
	int total = pack->GetPacketLen();
	char *buff = pack->GetBuffer();
	while (1)
	{
		if (m_IsValid == 0)
			return 0;
		int len = write(m_SocketFD, buff + count, total - count);
		if (len >= 0)
		{
			count += len;
			printf("write to %i packet len: %i\n", m_SocketFD, total);
			printf("the ip port is %s\n", SockFDToPeerString(m_SocketFD).c_str());
		}
		else
			break;
		if (count >= total)
		{
			printf("packet sent\n");
			break;
		}
	}
	delete pack;
	return 1;
}

void CRPCConnection::Close()
{
	close(m_SocketFD);
	m_IsClosed = 0;
	m_IsValid = 0;
}


// -------------------------- CRPCReader -------------------------

void * CRPCReader::RPCReaderThreadFunc(void *param)
{
	CRPCReader * reader = (CRPCReader *)param;
	printf("reader thread %d\n", pthread_self());
	if (reader == NULL)
		return NULL;
	string ipaddr;
    	while (true)
	{
		ipaddr = reader->GetNextIPAddr();
		if (reader->IsStop())
			break;
		else
		{
			printf("ip:%s  thread %d\n", ipaddr.c_str(), pthread_self());
			reader->Process(ipaddr);
		}
	}
	printf("reader thread %d exited\n", pthread_self());
	return 0;
}

CRPCReader::CRPCReader(CRPCReaderCallbacker *callback, int threadNum)
{
	m_ThreadNum = threadNum;
	m_Callback = callback;
	Init();
}

CRPCReader::~CRPCReader()
{
	Deinit();
}

void CRPCReader::Init()
{
	m_Threads = (pthread_t *)malloc(m_ThreadNum * sizeof(pthread_t));
	m_ReadQueue = new CRPCReadFDQueue();
	m_Stop = 0;
	m_State = 0;
	pthread_mutex_init(&m_Mutex, NULL);
	//m_Callback = NULL;
}

void CRPCReader::Deinit()
{
	free(m_Threads);
	m_Threads = NULL;
	delete m_ReadQueue;
	m_ReadQueue = NULL;
	pthread_mutex_destroy(&m_Mutex);
}

int CRPCReader::Start()
{
	pthread_mutex_lock(&m_Mutex);
	int ret = 1;
	do
	{
		// stop is called, so we don't start
		if (m_Stop == 1)
		{
			ret = 0;
			break;
		}
		// already running
		if (m_State == 1)
		{
			ret = 2;
			break;
		}
		m_State = 1;
		for (int idx = 0; idx < m_ThreadNum; idx++)
			pthread_create(&m_Threads[idx], NULL,
					   CRPCReader::RPCReaderThreadFunc, (void *)this);
	}
	while (0);
	pthread_mutex_unlock(&m_Mutex);
	return ret;
}

void CRPCReader::Stop(int stop)
{
	pthread_mutex_lock(&m_Mutex);
	do
	{
		// not started, no action
		if (m_State == 0)
			break;
		// stop is true
		if (stop != 1)
			break;
		m_Stop = stop;
		//if (m_ReadQueue)
		//	m_ReadQueue->Stop(m_Stop);
		// wake up the thread sleep on the queue condition
		if (m_ReadQueue)
			m_ReadQueue->Wakeup(m_ThreadNum);
		// wait all read thread return
		for (int idx = 0; idx < m_ThreadNum; idx++)
		{
			void *value;
			pthread_join(m_Threads[idx], &value);
		}
		printf("all readers stoped\n");
		m_State = 0;
		m_Stop = 0;
	}
	while (0);
	pthread_mutex_unlock(&m_Mutex);
}

int CRPCReader::IsStop()
{
	return m_Stop;
}

int CRPCReader::Process(string ipaddr)
{
	if (m_Callback == NULL)
	{
		printf("call back not exist\n");
		return -1;
	}
	CRPCConnection *conn = m_Callback->ReaderGetConnection(ipaddr);
	if (conn == NULL)
		return 0;
	//printf("process connection:%s\n", conn->GetIPAddress().c_str());
	int ret = conn->Read();
	if (ret <= 0)
	{
		m_Callback->ReaderRemoveFD(conn->GetSocketFD());
		conn->Close();
		m_Callback->ReaderRemoveConnection(conn);
	}
	return 1;
}


void CRPCReader::NewReadEvent(int fd)
{
	printf("new read event from %i\n", fd);
	if (m_ReadQueue)
		m_ReadQueue->Push(SockFDToPeerString(fd));
}

string CRPCReader::GetNextIPAddr()
{
	string ipaddr;
	if (m_ReadQueue && m_ReadQueue->Pop(ipaddr))
		return ipaddr;
	return "";
}

// -------------------------- CRPCWriter -------------------------

void * CRPCWriter::RPCWriterThreadFunc(void *param)
{
	CRPCWriter * writer = (CRPCWriter *)param;
	printf("writer thread %d\n", pthread_self());
   	while (true)
	{
		if (writer->Process() < 0)
			break;
	}
	printf("writer %d exited\n", pthread_self());
	return 0;
}

CRPCWriter::CRPCWriter(CRPCWriterCallbacker *callback, int threadnum)
{
	m_Callback = callback;
	map<string, CRPCConnection *> *connections = m_Callback->WriterGetConnections();
	m_ItNext = connections->begin();
	m_ThreadNum = threadnum;
	pthread_cond_init(&m_Cond, NULL);
	pthread_mutex_init(&m_Mutex, NULL);
	m_Stop = 0;
	m_Threads = (pthread_t *)malloc(m_ThreadNum * sizeof(pthread_t));
}

CRPCWriter::~CRPCWriter()
{
	pthread_cond_destroy(&m_Cond);
	pthread_mutex_destroy(&m_Mutex);
}

void CRPCWriter::WritePacket(string ip, CRPCPacket *packet)
{
	pthread_mutex_lock(&m_Mutex);
	CRPCConnection *pconn = m_Callback->WriterGetConnection(ip);
	if (pconn)
	{
		pconn->Write(packet);
		if (pconn->IsSending())
		{
			
		}
		else
		{
			pthread_cond_signal(&m_Cond);
		}
	}
	pthread_mutex_unlock(&m_Mutex);
}

CRPCConnection * CRPCWriter::GetConnectionAndWrite()
{
	pthread_mutex_lock(&m_Mutex);
	CRPCConnection *ret = NULL;
	while(1)
	{
		if (m_Stop == 1)
			break;
		printf("writer not stop\n");
		m_Callback->WriterLockConnections();
		map<string, CRPCConnection *> *connections = m_Callback->WriterGetConnections();
		// no connection go to sleep
		if (connections->size() == 0)
		{
			m_Callback->WriterUnLockConnections();
		}
		else
		{
			if (m_ItNext == connections->end())
			{
				m_ItNext = connections->begin();
			}
			// remark the current connection, to see if we meet it later
			map<string, CRPCConnection *>::iterator itold = m_ItNext;
			while (1)
			{
				printf("while\n");
				if (m_ItNext == connections->end())
				{
					m_ItNext = connections->begin();
					printf("set to begin()\n");
					continue;
				}
				CRPCConnection *pconn = m_ItNext->second;
				// get a connection, if it is not sending datas and there are datas need to be sent, we pick it
				if (!pconn->IsSending() && pconn->GetNumOfOutPackets() > 0)
				{
					ret = pconn;
					pconn->SetSending(1);
					m_ItNext++;
					m_Callback->WriterUnLockConnections();
					break;
				}
				// we meet the remarked connection, it means there isn't any available connections, go to sleep
				if (m_ItNext == itold)
				{
					printf("old repeat\n");
					m_Callback->WriterUnLockConnections();
					break;
				}
				printf("seek to next\n");
				m_ItNext++;
			}
		}
		
		if (ret == NULL)
		{
			printf("no more data to write\n");
			pthread_cond_wait(&m_Cond, &m_Mutex);
		}
		else
			break;
	}
	pthread_mutex_unlock(&m_Mutex);
	return ret;
}

int CRPCWriter::Process()
{
	CRPCConnection *conn = GetConnectionAndWrite();
	if (conn == NULL)
	{
		printf("writer stop is called, so empty data is emmit\n");
		return -1;
	}
	int ret = conn->Send();
	pthread_mutex_lock(&m_Mutex);
	conn->SetSending(0);
	pthread_mutex_unlock(&m_Mutex);
	if (m_Stop == 1)
		ret = -1;
	return ret;
}

void CRPCWriter::ConnectionRemoved(map<string, CRPCConnection *>::iterator it)
{
	pthread_mutex_lock(&m_Mutex);
	if (m_ItNext == it)
	{
		m_ItNext++;
	}
	pthread_mutex_unlock(&m_Mutex);
}
	
int CRPCWriter::Start()
{
	int ret = 1;
	do
	{
		for (int idx = 0; idx < m_ThreadNum; idx++)
			pthread_create(&m_Threads[idx], NULL,
					   CRPCWriter::RPCWriterThreadFunc, (void *)this);
	}
	while (0);
	return ret;
}

int CRPCWriter::Stop(int stop)
{
	do
	{
		m_Stop = stop;
		for (int idx = 0; idx < m_ThreadNum; idx++)
			pthread_cond_signal(&m_Cond);
		
		// wait all read thread return
		for (int idx = 0; idx < m_ThreadNum; idx++)
		{
			void *value;
			pthread_join(m_Threads[idx], &value);
		}
		printf("all writer stoped\n");
		m_Stop = 0;
	}
	while (0);
	return 1;
}

// -------------------------- CRPCKernel -------------------------
CRPCKernel::CRPCKernel(): CEpoll()
{
	Init();
}

CRPCKernel::CRPCKernel(int maxEvents): CEpoll(maxEvents)
{
	Init();
}

CRPCKernel::~CRPCKernel()
{
	Deinit();
}

void CRPCKernel::Init()
{
	m_Stop = 0;
	m_Reader = NULL;
	m_State = 0;
	pthread_mutex_init(&m_Mutex, NULL);
	m_Reader = new CRPCReader(this, READER_THREAD);
	m_Writer = new CRPCWriter(this, WRITER_THREAD);
	if (pipe(m_Pipe) < 0)
		printf("pipe failed\n");
}

void CRPCKernel::Deinit()
{
	delete m_Reader;
	m_Reader = NULL;
	delete m_Writer;
	m_Writer = NULL;
	pthread_mutex_destroy(&m_Mutex);
	close(m_Pipe[0]);
	close(m_Pipe[1]);
	memset(m_Pipe, 0, sizeof(m_Pipe));
}

int CRPCKernel::GetState()
{
	return m_State;
}

int CRPCKernel::EpollLoop()
{
	return 1;
}

void CRPCKernel::Start()
{
	if (m_State == 1)
		return;
	m_State = 1;
	m_Reader->Start();
	m_Writer->Start();
}

void CRPCKernel::Stop(int stop)
{
	m_Stop = stop;
	printf("stop is %i\n", m_Stop);
	if (m_Stop == 1)
	{
		int i = write(m_Pipe[1], "pipe stop\n", 9);
		printf("write to pipe %i errno: %i\n", i, errno);
	}
}

CRPCConnection * CRPCKernel::GetConnection(string ip_port)
{
	map<string, CRPCConnection *>::iterator it = m_Connections.find(ip_port);
	if (it == m_Connections.end())
	{
		printf("get connection %s not found\n", ip_port.c_str());
		return NULL;
	}
	return it->second;
}

CRPCConnection * CRPCKernel::GetConnection(int socketfd)
{
	string ip_port = SockFDToPeerString(socketfd);
	return GetConnection(ip_port);
}

CRPCConnection * CRPCKernel::GetConnection(struct sockaddr_in * peeraddr)
{
	string ip_port = SockaddrToString(peeraddr);
	return GetConnection(ip_port);
}

int CRPCKernel::ConnectTo(string ip_port)
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

int CRPCKernel::Disconnect(string ip_port)
{
	pthread_mutex_lock(&m_Mutex);
	map<string, CRPCConnection *>::iterator it = m_Connections.find(ip_port);
	//map<string, CRPCConnection *>::iterator it = m_Connections.find(conn->GetIPAddress().c_str());
	if (it != m_Connections.end())
	{
		RemoveFromEpoll(it->second->GetSocketFD(), EPOLLIN | EPOLLET);
		it->second->Close();
		m_Writer->ConnectionRemoved(it);
		delete it->second;
		m_Connections.erase(it);
	}
	pthread_mutex_unlock(&m_Mutex);
}

int CRPCKernel::AddConnection(int socketfd)
{
	printf("go in to add conn\n");
	pthread_mutex_lock(&m_Mutex);
	int ret = 0;
	string ip_port = SockFDToPeerString(socketfd);
	printf("add connection %s\n", ip_port.c_str());
	if (ip_port == "")
	{
		ret = -1;
	}
	else
	{
		map<string, CRPCConnection *>::iterator it = m_Connections.find(ip_port);
		if (it == m_Connections.end())
		{
			CRPCConnection *conn = new CRPCConnection(socketfd);
			m_Connections[ip_port] = conn;
			ret = 1;
		}
	}
	pthread_mutex_unlock(&m_Mutex);
	return ret;
}

int CRPCKernel::RemoveConnection(CRPCConnection *conn)
{
	pthread_mutex_lock(&m_Mutex);
	map<string, CRPCConnection *>::iterator it = m_Connections.find(conn->GetIPAddress().c_str());
	if (it != m_Connections.end())
	{
		m_Writer->ConnectionRemoved(it);
		delete it->second;
		m_Connections.erase(it);
	}
	pthread_mutex_unlock(&m_Mutex);
}

void CRPCKernel::ReaderRemoveConnection(CRPCConnection *conn)
{
	RemoveConnection(conn);
}

int CRPCKernel::ReaderRemoveFD(int fd)
{
	printf("server remove fd %d\n", fd);
	return RemoveFromEpoll(fd, EPOLLIN | EPOLLET);
}

CRPCConnection * CRPCKernel::ReaderGetConnection(string ipaddr)
{	
	//printf("reader get conn\n");
	return GetConnection(ipaddr);
}

map<string, CRPCConnection *> * CRPCKernel::WriterGetConnections()
{
	return &m_Connections;
}

CRPCConnection * CRPCKernel::WriterGetConnection(string ipaddr) 
{
	return GetConnection(ipaddr);
}

void CRPCKernel::WriterLockConnections()
{
	printf("writer lock\n");
	pthread_mutex_lock(&m_Mutex);
}

void CRPCKernel::WriterUnLockConnections()
{
	printf("writer unlock\n");
	pthread_mutex_unlock(&m_Mutex);
}
