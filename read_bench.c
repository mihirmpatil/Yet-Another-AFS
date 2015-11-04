#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <fcntl.h>

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

	char *filename = "/tmp/afs/bench/bench_file1";
	char *cached_filename = "/tmp/cache/bench/bench_file1";
	int i, fd;
	const int buf_size = 1024;
	char buffer[buf_size];
	int trials = 1;
	char *readtimes = "results/readtimes.txt";
	char *remoteopentimes = "results/remoteopentimes.txt";
	char *localopentimes = "results/localopentimes.txt";
//	FILE *readfd = fopen(readtimes, "w+");
//	FILE *localopenfd = fopen(localopentimes, "w+");
//	FILE *remoteopenfd = fopen(remoteopentimes, "w+");
	for (i = 0; i < trials; i++)
	{
		clock_gettime(CLOCK_REALTIME, &start);
		fd = open(filename, O_RDONLY);
		clock_gettime(CLOCK_REALTIME, &end);
		printf("Time for first open of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		write
		if (fd == -1)
			exit(1);
		close(fd);
		
		// next open should be served from the cache
		clock_gettime(CLOCK_REALTIME, &start);
		fd = open(filename, O_RDONLY);
		clock_gettime(CLOCK_REALTIME, &end);
		printf("Time for next open of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		if (fd == -1)
			exit(1);
		
		// Time for local read
		clock_gettime(CLOCK_REALTIME, &start);
		read(fd, buffer, buf_size);	
		clock_gettime(CLOCK_REALTIME, &end);
		printf("Time for next open of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		close(fd);

		// remove cached copy
		unlink(cached_filename);
	}
	return 0;
}
