/*
extern "C" 
{
  int grpc_afs_open(const char *path);
}
*/

extern int grpc_afs_open(const char *path);
extern struct afs_dirent* grpc_afs_readdir(const char *path);
extern int grpc_afs_getattr(const char *path, struct stat **s, const char *cache_path);
extern int grpc_afs_flush(const char *path);
extern int grpc_afs_mkdir(const char *path);
extern int grpc_afs_rmdir(const char *path);
extern int grpc_afs_unlink(const char *path);
extern int grpc_afs_rename(const char *from, const char *to);
