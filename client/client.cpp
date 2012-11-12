#include "Common.h"
#include <sys/socket.h> // socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_pton

int main()
{
	int fd;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		printf("socket failed\n");
		return 1;
	}
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, "192.168.199.128", &servaddr.sin_addr) <= 0)
	{
		printf("inet_pton failed\n");
		return 1;
	}
	if (connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect failed\n");
		return 1;
	}
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);
	getsockname(fd, (struct sockaddr *)&clientaddr, &addrlen);
	char buff[1024];
	for (int index = 0; index < 20; index++)
	{
		memset(buff, 0, 1024);
		sprintf(buff, "this is %d 's message from %s:%d\n", index + 1,
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		int sent = write(fd, buff, strlen(buff));
		printf("next %d\n", sent);
		if (index % 10 == 9)
			sleep(1);
	}
	//sleep(2);
	write(fd, "end", 3);
	sleep(1);
	close(fd);
	
	return 0;
}
