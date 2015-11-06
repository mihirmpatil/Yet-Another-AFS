#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define KB 1024

int main()
{
	char *filename = "/tmp/server/bench/bench_file2";
	int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC);
	
	int size = 1*KB*KB*KB;

	int i;
	for (i = 0; i < size; i++)
	{
		write(fd, "a", 1);
	}
	return 0;
}
