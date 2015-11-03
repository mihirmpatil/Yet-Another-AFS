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
	const int buf_size = 1025;
	char buffer[buf_size];
	for (i = 0; i < buf_size - 1; i++)
		buffer[i] = 'a';
	buffer[buf_size-1] = '\0';

	int trials = 1;
	for (i = 0; i < trials; i++)
	{
		fd = open(filename, O_WRONLY);
		if (fd == -1)
			exit(1);
		clock_gettime(CLOCK_REALTIME, &start);
		write(fd, buffer, buf_size);
		clock_gettime(CLOCK_REALTIME, &end);
		printf("Time for write of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		clock_gettime(CLOCK_REALTIME, &start);
		close(fd);
		clock_gettime(CLOCK_REALTIME, &end);
		printf("Time for flush of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		
	}
	return 0;
}
