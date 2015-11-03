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

void perform_write(char *filename);
{
	int i;
	const int buf_size = 1025;
	char data[buf_size];
	for (i = 0; i < buf_size - 1; i++)
		data[i] = 'b';
	data[buf_size] = '\0';
	int fd = open(filename, O_CREAT | O_RDWR);
	int size = write(fd, data, buf_size);
	close(fd);
}

int main(int argc, char **argv)
{
	pthread_t threads[THREADS];
	char *filename = "test_file"; //argv[1];
	int i;

	// comment out one out of read or write

	for (i = 0; i < THREADS; i++)
	{
		pthread_create(&threads[i], NULL, perform_read, filename);
	}
/*
	for (i = 0; i < THREADS; i++)
	{
		pthread_create(&threads[i], NULL, perform_write, filename);
	}
*/	
	for (i = 0; i< THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return 0;
}
