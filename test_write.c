#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd = open("/tmp/afs/random", O_RDWR | O_CREAT);
	char buf[5];
	buf[0] = '1';
	buf[1] = '2';
	buf[2] = '3';
	buf[3] = '4';
	buf[5] = '5';

	if(lseek(fd,0,SEEK_END) == -1){
	  printf("\nError setting offset");
	  return 0;
	}
	if(write(fd,buf,4) == -1){
	  printf("\nError writing to the file");
	  return 0;
	}
	printf("\nFile successfully written");
	close(fd);
	return 0;
}
