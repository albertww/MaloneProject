#include <fcntl.h>
#include <stdio.h>

int GetOpt(int fd)
{
	int opt = fcntl(fd, F_GETFL);
	return opt;
}

int main()
{
	int fd = open("common.h", O_RDONLY);
	printf("%d\n", GetOpt(fd));
	return 0;
}
