#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd = open("/tmp/afs/filetemp", O_RDONLY);
	printf("open succeeded\n");
	close(fd);
	return 0;
}
