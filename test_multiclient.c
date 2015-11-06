#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#define BUF_LEN 1024
#define THREADS 1
#define BILLION 1000000000L
int runs = 1;

uint64_t time_diff(struct timespec start, struct timespec end)
{
    return BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
}

void* perform_read(void *vfilename)
{
	char *filename = (char*)vfilename;
	char buffer[BUF_LEN];
	int size;
	int byteswritten, i;
	long long unsigned int diff;
    struct timespec start, end;


	clock_gettime(CLOCK_REALTIME, &start);
	int fd = open(filename, O_RDONLY);
	size = read(fd, buffer, BUF_LEN);
	clock_gettime(CLOCK_REALTIME, &end);
	diff = (long long unsigned int)time_diff(start, end);
	printf("%llu\t%s\n",diff,filename);
	//printf("%d - %s\n", getttid(), buffer);
	close(fd);
}

/*void perform_write(char *filename)
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
*/

void* perform_rwc(void *vfilename){
	char *filename = (char*)(vfilename);
	int bufsize = 1024 * 1024 * 1; // 8MB
	char *data = (char*)malloc(bufsize*sizeof(char));
	int byteswritten, fd,i;
	long long unsigned int diff;
    struct timespec start, end;

	for(i=0;i<bufsize-1;i++){
		data[i] = 'a' + i%26;
	}
	data[i] = '\0';

	for(i=0;i<runs;i++){
		clock_gettime(CLOCK_REALTIME, &start);
		//Open
		fd = open(filename, O_CREAT | O_RDWR);
		//Write
		byteswritten = write(fd,data,bufsize);
		//Close
		close(fd);
		//Delete
		unlink(filename);
		clock_gettime(CLOCK_REALTIME, &end);
		diff = (long long unsigned int)time_diff(start, end);
		printf("%s\t%llu\n",filename,diff);
	}
	free(data);
}

int main(int argc, char **argv)
{
	pthread_t threads[THREADS];
	char filenames[THREADS][12]; //argv[1];
	char *afs_path = "/tmp/afs/";
	int i;
	void *ptr;

	// comment out one out of read or write
/*
	for (i = 0; i < THREADS; i++)
	{
		pthread_create(&threads[i], NULL, perform_read, filename);
	}
*/
/*
	for (i = 0; i < THREADS; i++)
	{
		pthread_create(&threads[i], NULL, perform_write, filename);
	}
*/	

	for(i=0;i<THREADS;i++){
		strcpy(filenames[i],afs_path);
		filenames[i][9] = 'f';
		filenames[i][10] = 'a' + i;
		filenames[i][11] = '\0';
	}

	for(i=0;i<THREADS;i++){
		pthread_create(&threads[i], NULL, perform_read, (void*)filenames[i]);
	}
	
	for (i = 0; i< THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return 0;
}
