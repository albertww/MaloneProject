/* 
** Author: albert 
** Date: 2012-5-12
** Description: RPC common classes. There are 3 main class, RPCReader read data from socket,
** RPCWriter write data to socket, RPCConnection manage a socket connection
*/
#ifndef __RPC_COMMON_H__
#define __RPC_COMMON_H__
#include "Common.h"
#include "RPCPacket.h"
#include "StringUtility.h"
#include "Epoll.h"
#include <list>
#include <string>
#include <map>
using namespace std;

#define BUFF_LEN MAX_RPC_PACKET_LEN * 2
#define READER_THREAD 3
#define WRITER_THREAD 2

template <class T>
class CThreadSafeQueue
{
public:
	CThreadSafeQueue();
	virtual ~CThreadSafeQueue();
	// @param data to be put in queue
	// @return 1 suc, 0 failed
	virtual int Push(const T &data);
	// @param data, fetch the next ipaddr
	// @return 1 suc, 0 failed
	virtual int Pop(T &data);
	int Size();
protected:
	list<T> m_Queue;
	pthread_mutex_t m_Mutex;
	pthread_cond_t m_Cond;
};

class CRPCReadFDQueue: CThreadSafeQueue<string>
{
public:
	CRPCReadFDQueue();
	virtual ~CRPCReadFDQueue();
	// @param ipaddr to be put in queue
	// @return 1 suc, 0 failed
	virtual int Push(const string &data);
	// @param ipaddr, fetch the next ipaddr
	// @return 1 suc, 0 failed
	virtual int Pop(string &data);
	void Wakeup(int num);
};

class CConnectionReadBuffer
{
public:
	CConnectionReadBuffer();
	void Reset();
	void Adjust();
	int Remainder();
	int GetMaxBuffLen();
	char * GetWholeBuffer();
	// @return, -1 failed, 0 client send eof, 1 no more data can be read on this fd
	// change to read another socket
	int Read(int fd, list<CRPCPacket *> &li);
private:
	CRPCPacket * GetPacket();

private:
	int m_Cursor;
	int m_Len;
	char m_Buff[BUFF_LEN];
};

class CRPCConnection
{
public:
	CRPCConnection(int fd);
	~CRPCConnection();
	string GetIP();
	string GetPort();
	string GetIPAddress();
	string GetLocalAddress();
	int GetSocketFD();
	int IsValid();
	int GetDirection();
	// @return -1 error, 0 read to end, 1 no data in kernel buffer to be read
	int Read();
	int Write(CRPCPacket *packet);
	int IsSending();
	void SetSending(int flag);
	int GetNumOfOutPackets();
	// send one of its packet
	// @return 1 send a packet successfully, 0 not send, 
	// may be connection closed or sth else
	int Send();
	void Close();
private:
	// ip:port
	string m_IPAddress;
	string m_LocalIPAddress;
	struct sockaddr_in m_PeerAddr;
	int m_SocketFD;
	int m_IsValid;
	int m_IsClosed;
	int m_IsSending;
	int m_Direction;
	CConnectionReadBuffer m_PacketBuffer;
	list<CRPCPacket *> m_InPackets;
	CThreadSafeQueue<CRPCPacket *> m_OutPackets;
};

class CRPCReaderCallbacker
{
public:
	// inform epollserver remove a fd from epoll, cause either error or eof, the fd is closed
	virtual int ReaderRemoveFD(int fd) {}
	virtual void ReaderRemoveConnection(CRPCConnection *conn) {}
	virtual CRPCConnection * ReaderGetConnection(string ipaddr) {}
};

class CRPCReader
{
public:
	static void * RPCReaderThreadFunc(void *param);
public:
	CRPCReader(CRPCReaderCallbacker *callback, int threadNum);
	~CRPCReader();
	void Init();
	void Deinit();
	// @return 0 failed, 1 ok, 2 already start
	int Start();
	void Stop(int stop);
	int IsStop();
	int Process(string ipaddr);
	void NewReadEvent(int fd);
private:
	string GetNextIPAddr();
private:
	int m_ThreadNum;
	// only two values, 0 don't stop, 1 must stop
	int volatile m_Stop;
	// only two state, 0 not started, 1 started
	int m_State;
	pthread_t *m_Threads;
	CRPCReadFDQueue *m_ReadQueue;
	pthread_mutex_t m_Mutex;
	CRPCReaderCallbacker *m_Callback;
};

class CRPCWriterCallbacker
{
public:
	virtual map<string, CRPCConnection *> * WriterGetConnections() {}
	virtual CRPCConnection * WriterGetConnection(string ipaddr) {}
	virtual void WriterLockConnections() {}
	virtual void WriterUnLockConnections() {}	
};

class CRPCWriter
{
public:
	static void * RPCWriterThreadFunc(void *param);
public:
	CRPCWriter(CRPCWriterCallbacker *callback, int threadnum);
	~CRPCWriter();
	void WritePacket(string ip, CRPCPacket *packet);
	CRPCConnection * GetConnectionAndWrite();
	int Process();
	void ConnectionRemoved(map<string, CRPCConnection *>::iterator it);
	int Start();
	int Stop(int stop);
	
private:
	map<string, CRPCConnection *>::iterator m_ItNext;
	pthread_mutex_t m_Mutex;
	pthread_cond_t m_Cond;
	pthread_t *m_Threads;
	int m_ThreadNum;
	CRPCWriterCallbacker *m_Callback;
	int m_Stop;
};

class CRPCKernel: public CEpoll, public CRPCReaderCallbacker, public CRPCWriterCallbacker
{
public:
	CRPCKernel();
	CRPCKernel(int maxEvents);
	virtual ~CRPCKernel();
	void Init();
	void Deinit();
	int GetState();
	virtual int EpollLoop();
	void Start();
	void Stop(int stop);
	CRPCConnection * GetConnection(string ip_port);
	CRPCConnection * GetConnection(int socketfd);
	CRPCConnection * GetConnection(struct sockaddr_in * peeraddr);
	int ConnectTo(string ip_port);
	int Disconnect(string ip_port);
	// caution: everytime add a connection or remove a connection, the map must be protected from multi-thread
	// the mutex must be used
	// @return -1 failed, 0 already have connection, 1 add suc
	int AddConnection(int socketfd);
	int RemoveConnection(CRPCConnection *conn);
	// @override CRPCReaderCallbacker
	void ReaderRemoveConnection(CRPCConnection *conn) ;
	// @override CRPCReaderCallbacker
	int ReaderRemoveFD(int fd);
	// @override CRPCReaderCallbacker
	CRPCConnection * ReaderGetConnection(string ipaddr);
	// @override CRPCWriterCallbacker
	map<string, CRPCConnection *> * WriterGetConnections();
	// @override CRPCWriterCallbacker
	CRPCConnection * WriterGetConnection(string ipaddr);
	// @override CRPCWriterCallbacker
	void WriterLockConnections();
	// @override CRPCWriterCallbacker
	void WriterUnLockConnections();
protected:
	int m_Pipe[2];
	int m_State;
	// 0 don't stop, 1 stop epollserver
	int m_Stop;
	CRPCReader *m_Reader;
	CRPCWriter *m_Writer;
	pthread_mutex_t m_Mutex;
	// reserve all the connections, the key is a string of peer "ip:port"
	map<string, CRPCConnection *> m_Connections; 
	
};

#endif
