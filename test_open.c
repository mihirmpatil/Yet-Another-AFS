#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd = open("tmp/client/random_file", O_RDWR | O_CREAT);
	printf("open succeeded\n");
	return 0;
}