#include <iostream>
#include <cstring>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

using namespace std;

int main(){
  string cmd = "/tmp/afs/test/testfolder";
  printf("\nmk Return = %d",mkdir(cmd.c_str(),S_IRWXU));
  printf("\nrm Return = %d",rmdir(cmd.c_str()));
  return 0;
}
