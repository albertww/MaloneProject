#include "Epoll.h"

// ------------------------- CEpoll ------------------------------

CEpoll::CEpoll(): m_Connections(0), m_EpollFD(-1), m_FDTriggered(0), m_Iterator(-1)
{
	m_MaxEvent = DEFAULT_EPOLL_MAX_EVENT;
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
		m_MaxEvent = DEFAULT_EPOLL_MAX_EVENT;
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

int CEpoll::AddToEpoll(int fd, int event)
{
	int ret = 0;
	pthread_mutex_lock(&m_Mutex);
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = event;
	if (0 == epoll_ctl(m_EpollFD, EPOLL_CTL_ADD, fd, &ev))
	{
		ret = 1;
		m_Connections++;
	}
	pthread_mutex_unlock(&m_Mutex);
	return ret;
}

int CEpoll::RemoveFromEpoll(int fd, int event)
{
	int ret = 0;
	pthread_mutex_lock(&m_Mutex);
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = event;
	if (0 == epoll_ctl(m_EpollFD, EPOLL_CTL_DEL, fd, &ev))
		ret = 1;
	if (ret == 1)
	{
		printf("remove suc\n");
		m_Connections--;
	}
	else
		printf("remove fail, sockfd %i, errorno %i\n", fd, errno);
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

int CEpoll::EpollLoop()
{
	
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
		memcpy(event, (void*)&m_EpollEvents[m_Iterator], 
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
