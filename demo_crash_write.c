#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char *filename = "/tmp/afs/file123";
	int data_size = 1025;
	char buffer[data_size];
	int i;
	for (i = 0; i < data_size - 1; i++)
		buffer[i] = 'a';
	buffer[i] = '\0';

	char x;
	int fd = open(filename, O_WRONLY);
	lseek(fd, 0, SEEK_END);
	int num_writes = 3;
	for (i = 0; i < num_writes; i++)
	{
		write(fd, buffer, data_size-1);
		write(fd, "next\n", 6);
		if (i == 1)
		{
			scanf("%c",&x);
		}
	}
	return 0;
}
