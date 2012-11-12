#include "Common.h"

// -------------------------- common functions -----------------------------

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

int SetMaxOpenFD(int num)
{
	struct rlimit rt;
	if (getrlimit(RLIMIT_NOFILE, &rt) < 0)
		return 0;
	rt.rlim_max = rt.rlim_cur = num;
	if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
		return 0;
	else
		return 1;
}

string SockaddrToString(struct sockaddr_in * sa)
{
	char * ipstr = inet_ntoa(sa->sin_addr);
	if (ipstr == NULL)
		return "";
	string ip = ipstr;
	string port = Itoa(ntohs(sa->sin_port));
	string ip_port = ip + ":" + port;
	return ip_port;
}

string SockFDToPeerString(int socketfd)
{
	struct sockaddr_in sa;
	//int len = sizeof(sa);
	socklen_t len = sizeof(sa);
	if (0 == getpeername(socketfd, (struct sockaddr *)&sa, &len))
	{
		return SockaddrToString(&sa);
	}
	return "";
}

string SockFDToLocalString(int socketfd)
{
	struct sockaddr_in sa;
	socklen_t addrlen = sizeof(sa);
	if (0 == getsockname(socketfd, (struct sockaddr *)&sa, &addrlen))
	{
		return SockaddrToString(&sa);
	}
	return "";
}

int Itoa(int val, char* buf)
{
    const unsigned int radix = 10;
    char* p;
    unsigned int a;        //every digit
    int len;
    char* b;            //start of the digit char
    char temp;
    unsigned int u;
    p = buf;
    if (val < 0)
    {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned int)val;
    b = p;
    do
    {
        a = u % radix;
        u /= radix;
        *p++ = a + '0';

    } while (u > 0);
    len = (int)(p - buf);
    *p-- = 0;
    //swap
    do
    {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);
    return len;
}

char * Itoa(int val)
{
	static char buff[32];
	memset(buff, 0, sizeof(buff));
	Itoa(val, buff);
	return buff;
}

string GetIP(string raw)
{
	int i = raw.find(':');
	if (-1 == i)
		return "";
	return raw.substr(0, i);
}

string GetPort(string raw)
{
	int i = raw.find(':');
	if (-1 == i)
		return "";
	return raw.substr(i + 1);
}
