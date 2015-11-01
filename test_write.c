#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd = open("/tmp/afs/random2", O_RDWR | O_CREAT);
	if (fd == -1)
		printf("open call failed\n");
	char buf[5];
	buf[0] = '1';
	buf[1] = '2';
	buf[2] = '3';
	buf[3] = '4';
	buf[5] = '5';

	if(lseek(fd,0,SEEK_END) == -1){
	  printf("\nError setting offset\n");
	  return 0;
	}
	if(write(fd,buf,4) == -1){
	  printf("\nError writing to the file\n");
	  return 0;
	}
	printf("\nFile successfully written\n");
	close(fd);
	return 0;
}
