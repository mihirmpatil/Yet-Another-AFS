#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int main(){
  struct stat s;
  lstat("/tmp/afs/tmp/server/random_file",&s);

  cout<<"s.st_dev:"<<s.st_dev<<endl;
  cout<<"s.st_ino;"<<s.st_ino<<endl;
  cout<<"st_mode:"<<s.st_mode<<endl;
  cout<<"st_nlink:"<<s.st_nlink<<endl;
  cout<<"st_uid:"<<s.st_uid<<endl;
  cout<<"st_gid:"<<s.st_gid<<endl;
  cout<<"st_rdev:"<<s.st_rdev<<endl;
  cout<<"st_size:"<<s.st_size<<endl;
  cout<<"st_atime:"<<s.st_atime<<endl;
  cout<<"st_mtime:"<<s.st_mtime<<endl;
  cout<<"st_ctime:"<<s.st_ctime<<endl;
  cout<<"st_blksize:"<<s.st_blksize<<endl;
  cout<<"st_blocks:"<<s.st_blocks<<endl;

  return 0;
}
