/* 
** Author: albert 
** Date: 2012-2-24
** This is a muti-thread network server use epoll
*/

/*
	set a socket to be nonblock mode
	@param sockfd socket to be set
	@return 1 suc, 0 failed
*/
int SetNonblock(int sockfd)
{
	int opts = fcntl(sockfd, F_GETFL, 0);
	if (opts < 0)
		return 0;
	opts = opts | O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, opts) < 0)
		return 0;
	return 1;
}

#define DEFAULT_EPOLL_EVENT 5000
class CEpoll
{
public:
	CEpoll();
	// @param maxEvents, max event this epoll can process, 
	// it is also the max pollsize when we used to create epoll
	CEpoll(int maxEvents);
	virtual ~CEpoll();
	// add a file descriptor to this epoll
	// @param fd, file descriptor
	// @return 1 suc
	int AddToEpoll(int fd);
	// @param fd, file descriptor
	// @return, 1 suc
	int RemoveFromEpoll(int fd);
	// @param timeout, -1 forever wait, 0 return immidiately, 
	// or return after dedicated seconds
	// @return, -1 error, 0 timeout, or number of triggered fd returned
	int EpollWait(int timeout = -1);
	// is there some fd returned after EpollWait
	// @return 1 yes, 0 no
	int HaveNextFD();
	// get the next event
	// @return 0 failed, 1 suc
	int GetNextEvent(struct epoll_event *event);
	int GetConnections();
protected:
	int m_Connections;
	// file descrip
	int m_EpollFD;
	int m_MaxEvent;
	// number of file descriptors returned after wait
	int m_FDTriggered;
	// Iterator point to next triggered FD
	int m_Iterator;
	struct epoll_event *m_EpollEvents;
	pthread_mutex_t m_Mutex;
}



class CEpollThreadCallBacker
{
public:
	//CEpollThreadCallBacker();
	//~CEpollThreadCallBacker
	// which fd and what event on that fd is polled
	virtual void FDTriggered(int fd, int pollEvent);
}

#define EPOLL_TIMEOUT 5
class CEpollThread: public CEpoll
{
public:
	static void * EpollThreadRun(void *param);
public:
	CEpollThread();
	CEpollThread(maxEvents);
	~CEpollThread();
	void Start();
	void SetCallBack(CEpollThreadCallBacker *pCallBack);
	void SetStop(int stop);
	int Process();
	void NotifyCallBacker(int fd, int pollEvent);
protected:
	CEpollThreadCallBacker *m_CallBacker;
	// 1 to stop this thread
	int m_Stop;
	pthread_t m_ThreadID;
	int m_State;
}

#define DEFAULT_READERS 2
class CNIOReader: CEpollThreadCallBacker
{
public:
	CNIOReader(int readers = DEFAULT_READERS);
	void Start();
	void Stop();
	void AddFD(int fd);
	void RemoveFD(int fd);
private:
	CEpollThread* m_ReaderThreads;
	int m_ReaderCount;
	int m_State;
}