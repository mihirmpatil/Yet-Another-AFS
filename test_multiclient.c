#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>

#define BUF_LEN 1024
#define THREADS 5

void perform_read(char *filename)
{
	int fd = open(filename, O_RDONLY);
	char buffer[BUF_LEN];
	int size = read(fd, buffer, BUF_LEN);
	printf("%d - %s\n", getttid(), buffer);
	close(fd);
}

void perform_write(char *filename, void *data, int size);
{
	int fd = open(filename, O_CREAT | O_RDWR);
	int size = write(fd, data, size);
	close(fd);
}

int main(int argc, char **argv)
{
	pthread_t threads[THREADS];
	char *filename = "test_file"; //argv[1];
	int i;
	for (i = 0; i < THREADS; i++)
	{
		pthread_create(&threads[i], NULL, perform_read, filename);
	}

	for (i = 0; i< THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return 0;
}
