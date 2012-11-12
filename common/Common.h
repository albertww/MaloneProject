/* 
** Author: albert 
** Date: 2012-2-26
** This is a muti-thread network server use epoll
*/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h> // malloc, free
#include <stdio.h> // printf
#include <fcntl.h> // fcntl
#include <string.h> // memcpy
#include <sys/resource.h> // set/getrlimit
#include <sys/socket.h> // socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_pton
#include <errno.h> // errno
#include <pthread.h> // pthread_mutex
#include <unistd.h> // close

#include <string>
using namespace std;

#define SERVER_PORT 9988

/*
	set a socket to be nonblock mode
	@param sockfd socket to be set
	@return 1 suc, 0 failed
*/
int SetNonblock(int sockfd);
/*
	set number of file can be opened by process
	@param num number of fd can be opened by process
	@return 1 suc, 0 failed
*/
int SetMaxOpenFD(int num);

string SockaddrToString(struct sockaddr_in * sa);

string SockFDToPeerString(int socketfd);

string SockFDToLocalString(int socketfd);

int Itoa(int val, char *buf);

char * Itoa(int val);

string GetIP(string raw);

string GetPort(string raw);

#endif
