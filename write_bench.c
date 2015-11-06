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

	char *filename = "/tmp/afs/bench/bench_file2";
	char *cached_filename = "/tmp/cache/bench/bench_file2";
	int i, fd, j;
	const int buf_size = 1*1024 + 1; //1KB
	const int count = 10;

	char *buffer = (char*)malloc(buf_size*sizeof(char));
	for (j = 0; j < buf_size - 1; j++)
			buffer[j] = 'a';
	buffer[buf_size-1] = '\0';

	int trials = 10;
	for (i = 0; i < trials; i++)
	{
		int j;
		
		fd = open(filename, O_WRONLY |  O_CREAT);
		if (fd == -1)
			exit(1);
		j = count;
		clock_gettime(CLOCK_REALTIME, &start);
		while(j--){
			write(fd, buffer, buf_size);
		}
		clock_gettime(CLOCK_REALTIME, &end);
		long long unsigned int diff1 = (long long unsigned int)time_diff(start, end);
		//printf("Time for write of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		clock_gettime(CLOCK_REALTIME, &start);
		close(fd);
		clock_gettime(CLOCK_REALTIME, &end);
		long long unsigned int diff2 = (long long unsigned int)time_diff(start, end);
		//printf("Time for flush of %s - %llu\n", filename, (long long unsigned int)time_diff(start,end));
		printf("%llu\t%llu\n", diff1, diff2);
		unlink(filename);

	}
	free(buffer);
	return 0;
}
