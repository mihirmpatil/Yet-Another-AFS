#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <fcntl.h>

#define KB 1024
#define BILLION 1000000000L
//#define diff(start,end) BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

uint64_t time_diff(struct timespec start, struct timespec end)
{
	return BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
}

int main()
{
	uint64_t diff;
	struct timespec start, end;

	char *filename = "/tmp/afs/file10MB";
	char *cached_filename = "/tmp/cache/file10MB";
	int i, fd;
	const int buf_size = 10*1024*1024;
	//	char buffer[buf_size];
	int trials = 10;
	char *readtimes = "results/readtimes.txt";
	char *remoteopentimes = "results/remoteopentimes.txt";
	char *localopentimes = "results/localopentimes.txt";
//	FILE *readfd = fopen(readtimes, "w+");
//	FILE *localopenfd = fopen(localopentimes, "w+");
//	FILE *remoteopenfd = fopen(remoteopentimes, "w+");
	for (i = 0; i < trials; i++)
	{
		char *buffer = (char*)malloc(buf_size*sizeof(char));
		clock_gettime(CLOCK_REALTIME, &start);
		fd = open(filename, O_RDONLY);
		clock_gettime(CLOCK_REALTIME, &end);
		//printf("Time for first open of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		uint64_t diff1 = time_diff(start,end);
		if (fd == -1)
			exit(1);
		//close(fd);
		
		// next open should be served from the cache
		clock_gettime(CLOCK_REALTIME, &start);
		fd = open(filename, O_RDONLY);
		clock_gettime(CLOCK_REALTIME, &end);
		//printf("Time for next open of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		uint64_t diff2 = time_diff(start,end);

		if (fd == -1)
			exit(1);
		
		// Time for local read
		clock_gettime(CLOCK_REALTIME, &start);
		read(fd, buffer, buf_size);	
		clock_gettime(CLOCK_REALTIME, &end);
		//printf("Time for read of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		uint64_t diff3 = time_diff(start,end);
		close(fd);
		printf("%llu\t%llu\t%llu\n", (long long unsigned int)diff1, (long long unsigned int)diff2, (long long unsigned int)diff3);
		// remove cached copy
		char x;
		//scanf("%c",&x);
		unlink(cached_filename);
		free(buffer);
	}
	return 0;
}
