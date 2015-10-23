/*
extern "C" 
{
  int grpc_afs_open(const char *path);
}
*/

extern int grpc_afs_open(const char *path);
extern struct afs_dirent* grpc_afs_readdir(const char *path);
