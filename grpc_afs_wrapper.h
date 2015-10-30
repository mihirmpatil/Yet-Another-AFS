/*
extern "C" 
{
  int grpc_afs_open(const char *path);
}
*/

extern int grpc_afs_open(const char *path);
extern struct afs_dirent* grpc_afs_readdir(const char *path);
extern struct stat* grpc_afs_getattr(const char *path, struct stat *s);
extern int grpc_afs_flush(const char *path);
