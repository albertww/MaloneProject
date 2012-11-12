#include "epollserver.h"

CEpoll::CEpoll(): m_Connections(0), m_EpollFD(-1), m_FDTriggered(0), m_Iterator(-1)
{
	m_MaxEvent = DEFAULT_EPOLL_EVENT;
	pthread_mutex_init(&m_Mutex,NULL);
	m_EpollFD = epoll_create(m_MaxEvent);
	m_EpollEvents = (struct epoll_event *)malloc(m_MaxEvent * sizeof(struct epoll_event));
}

CEpoll::CEpoll(int maxEvents): m_Connections(0), m_EpollFD(-1), m_FDTriggered(0),
								m_Iterator(-1)
{
	if (maxEvents > 0)
		m_MaxEvent = maxEvents;
	else
		m_MaxEvent = DEFAULT_EPOLL_EVENT;
	pthread_mutex_init(&m_Mutex,NULL);
	m_EpollFD = epoll_create(m_MaxEvent);
	m_EpollEvents = (struct epoll_event *)malloc(m_MaxEvent * sizeof(struct epoll_event));
}

CEpoll::~CEpoll()
{
	pthread_mutex_destroy(&m_Mutex);
	free(m_EpollEvents);
	m_EpollEvents = NULL;
	close(m_EpollFD);
}

int CEpoll::AddToEpoll(int fd)
{
	int ret = 0;
	pthread_mutex_lock(&m_Mutex);
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET;
	if (0 == epoll_ctl(m_EpollFD, EPOLL_CTL_ADD, fd, &ev))
		ret = 1;
	m_Connections++;
	pthread_mutex_unlock(&m_Mutex);
	return ret;
}

int CEpoll::RemoveFromEpoll(int fd)
{
	int ret = 0;
	pthread_mutex_lock(&m_Mutex);
	struct epoll_event ev;
	//ev.data.fd = fd;
	//ev.events = EPOLLIN | EPOLLET;
	if (0 == epoll_ctl(m_EpollFD, EPOLL_CTL_ADD, fd, &ev))
		ret = 1;
	m_Connections--;
	pthread_mutex_unlock(&m_Mutex);
	return ret;
}

int CEpoll::EpollWait(int timeout)
{
	m_FDTriggered = epoll_wait(m_EpollFD, m_EpollEvents, m_Connections, timeout);
	// time out
	if (0 >= m_FDTriggered)
		m_Iterator = -1;
	else
		m_Iterator = 0;
	return m_FDTriggered;
}

int CEpoll::HaveNextFD()
{
	if (m_FDTriggered > 0 && m_Iterator < m_FDTriggered)
		return 1;
	return 0;
}

int CEpoll::GetNextEvent(struct epoll_event *event)
{
	if (m_FDTriggered > 0 && m_Iterator < m_FDTriggered)
	{
		memcpy(event, m_EpollEvenets[m_Iterator], 
			sizeof(struct epoll_event));
		m_Iterator++;
		return 1;
	}
	return 0;
}

int CEpoll::GetConnections()
{
	return m_Connections;
}

//--------------------------------------------------------

static void * CEpollThread::EpollThreadRun(void *param)
{
	CEpollThread *thread = (CEpollThread *)param;
	//pthread_exit((void *)thread->Process());
	return (void *)thread->Process();
}

CEpollThread::CEpollThread()
{
	m_CallBacker = NULL;
	m_Stop = 0;
	m_State = 0;
}

CEpollThread::CEpollThread(maxEvents): CEpoll(maxEvents)
{
	m_CallBacker = NULL;
	m_Stop = 0;
	m_State = 0;
}

CEpollThread::~CEpollThread()
{
}

void CEpollThread::Start()
{
	// already started
	if (1 == m_State)
		return;
	// not started, start
	if (m_State = 0)
		pthread_create(&m_ThreadID, NULL, EpollThreadRun, (void *)this);
	m_State = 1;
}

void CEpollThread::SetCallBack(CEpollThreadCallBacker *pCallBack)
{
	m_CallBacker = pCallBack;
}

void CEpollThread::SetStop(int stop)
{
	m_Stop = stop;
}

int CEpollThread::Process()
{
	int ret = 0;
	while (true)
	{
		if (1 == m_Stop)
			break;
		int fds = EpollWait(EPOLL_TIMEOUT);
		if (fds < 0)
		{
			ret = fds;
			break;
		}
		if (fds > 0)
		{
			while(HaveNextFD())
			{
				struct epoll_event event;
				GetNextEvent(&event);
				NotifyCallBacker(event.data.fd, event.events);
			}
		}
	}
	
	return ret;
}

void CEpollThread::NotifyCallBacker(int fd, int pollEvent)
{
	if (m_CallBacker)
		m_CallBacker->FDTriggered(fd, pollEvent);
}

// ------------------------------------------------

CNIOReader *CNIOReader::GetInstance()
{
	if (m_sInstance != NULL)
		return m_sInstance;
	m_sInstance = new CNIOReader();
	if (m_sInstance && m_sInstance.InitNIOReader())
		return m_sInstance;
	// no instance newed
	if (NULL == m_sInstance)
		return NULL;
	// allocated but init fail
	else
		delete m_sInstance;
	
}