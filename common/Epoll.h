/*
** Author: Albert
** Date: 2012-4-21
** Description: encapsulate the epoll
*/

#ifndef __CEPOLL_H__
#define __CEPOLL_H__
#include <sys/epoll.h>
#include "Common.h"

#define DEFAULT_EPOLL_MAX_EVENT 5000
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
	int AddToEpoll(int fd, int event);
	// @param fd, file descriptor
	// @return, 1 suc
	int RemoveFromEpoll(int fd, int event);
	// @param timeout, -1 forever wait, 0 return immidiately, 
	// or return after dedicated seconds
	// @return, -1 error, 0 timeout, or number of triggered fd returned
	int EpollWait(int timeout = -1);
	virtual int EpollLoop();
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
};

#endif