#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#include "afs_fuse_structs.h"
#include "grpc_afs_wrapper.h"

#define SUCCESS 0


char cache_path[1024];

// TODO implement this function
static int afs_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char prefixed_path[1024];

	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);
	
	if(grpc_afs_getattr(path, &stbuf, cache_path) == -1) {
		// Not present in server but in client
		if(lstat(prefixed_path, stbuf) == -1){
			return -errno;
		}
	}
	return 0;
}

static int afs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
					   off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;

	char prefixed_path[1024];

	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	printf("\nReaddir for path: %s\n",path);

	struct afs_dirent_array dirent_array = grpc_afs_readdir(path);
	int count = dirent_array.count;
	int i;

	printf("\nCount = %d",count);

	for (i = 0; i < count; i++) {
		struct stat st;

		memset(&st, 0, sizeof(st));
		printf("\nName: %s, mode %d, ino:%d",dirent_array.dirent_arr[i].name, dirent_array.dirent_arr[i].d_type,dirent_array.dirent_arr[i].reclen);
		st.st_ino = dirent_array.dirent_arr[i].reclen;
		st.st_mode = dirent_array.dirent_arr[i].d_type << 12;

		if (filler(buf, dirent_array.dirent_arr[i].name, &st, 0))
			break;
	}
	
	return 0;
}

/*
 * Given the path and cache path, returns the depth till which folders
 * exist.
 * Example: If path is /test_dir/foo/bar/file.txt and if server has hierarchy
 * /test_dir/foo, the function returns 2
 */
int existsInServer(const char *path, const char *cache_path){

	char curr_path[1024];
	struct stat *stbuf = (struct stat*)malloc(sizeof(struct stat));
	int i,count=0;

	curr_path[0] = '/';
	curr_path[1] = '\0';

	for(i=1;path[i]!='\0';i++){
		if(path[i] == '/'){
			// Check in server
			if(grpc_afs_getattr(curr_path, &stbuf, cache_path) == -1){
				return count;
			}
			else{
				curr_path[i] = path[i];
				curr_path[i+1] = '\0';
				count++;
			}
		}
		else{
			curr_path[i] = path[i];
			curr_path[i+1] = '\0';
		}
	}

	return count;
}

/*
 * Given the path, returns the no of folders in path
 * If path: /testdir/foo/bar/file1.txt -> returns 3
 */
int getDepth(const char *path){
	int i;
	int count=0;

	for(i=1;path[i]!='\0';i++){
		if(path[i] == '/'){
			count++;
		}
	}
	return count;
}

/*
 * Produces paths to individual folders in the hierarchy in order to help
 * with mkdir command. Example if path: /test_dir/foo/bar/testfile.txt
 * and cache_path: /tmp/cache then function populates split_paths as such
 * split_paths[0] = /tmp/cache/test_dir
 * split_paths[1] = /tmp/cache/test_dir/foo
 * split_paths[2] = /tmp/cache/test_dir/foo/bar
 */
void getSplitPaths(const char *path, const char *cache_path, char split_paths[][256]){
	int i,count=0;
	char temp[1024];

	split_paths[count][0] = '/';

	for(i=1;path[i]!='\0';i++){
		if(path[i] == '/'){
			// Set the current path correctly
			split_paths[count][i] = '\0';

			count++;
			strcat(split_paths[count],split_paths[count-1]);
			split_paths[count][i] = '/';
			split_paths[count][i+1] = '\0';
		}
		else{
			split_paths[count][i] = path[i];
			split_paths[count][i+1] = '\0';
		}
	}

	for(i=0;i<count;i++){
		strcpy(temp,cache_path);
		strcat(temp,split_paths[i]);
		strcpy(split_paths[i],temp);
	}
}

static int afs_open(const char *path, struct fuse_file_info *fi)
{
	char prefixed_path[1024];
	int s_hierarchy_depth, actual_depth,i;
	char split_paths[100][256];

	for(i=0;i<100;i++)
		memset(split_paths[i],'\0',256*sizeof(char));

	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	struct stat *stbuf = (struct stat*)malloc(sizeof(struct stat));
	struct stat *stbuf2 = (struct stat*)malloc(sizeof(struct stat));

	// Check in remote if the directory hierarchy exists
	s_hierarchy_depth = existsInServer(path, cache_path);
	actual_depth = getDepth(path);
	getSplitPaths(path, cache_path, split_paths);

	printf("\nActual Depth: %d, Server_Hierarchy_depth: %d",actual_depth,s_hierarchy_depth);
	for(i=0;i<actual_depth;i++){
		printf("\nSplitpath: %d = %s",i,split_paths[i]);
	}

	// Expected directory hierarchy is present in server
	if(actual_depth == s_hierarchy_depth){

		for(i=0;i<actual_depth;i++){
			// Make directory in local if it does not exist
			if(lstat(split_paths[i], stbuf) == -1){
				mkdir(split_paths[i], S_IRWXU);
			}
		}
	}
	else{
		// The directory structure does not exist in server
		// Remove any exisiting directories
		strcpy(prefixed_path,"rm -r -f ");
		strcat(prefixed_path,split_paths[s_hierarchy_depth]);

		printf("\nRemoving command = %s",prefixed_path);
		system(prefixed_path);
		return -1;
	}

	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	int local_ret = lstat(prefixed_path, stbuf);
	int remote_ret = grpc_afs_getattr(path, &stbuf2, cache_path);

	// file exists on both server and client
	if (local_ret != -1 && remote_ret != -1){
		// check if client has latest, otherwise fetch from server
		if (stbuf->st_mtime < stbuf2->st_mtime){
			unlink(prefixed_path);
			grpc_afs_open(path);
		}
		else
			open(prefixed_path,O_RDWR | O_CREAT);
	}
	// file doesn't exist anywhere
	else if (local_ret == -1 && remote_ret == -1){
		printf("\nFile does not exist anywhere");
		open(prefixed_path, O_RDWR | O_CREAT, S_IRWXU);
	}
	// file only exists locally
	else if (local_ret != -1 && remote_ret == -1){
		printf("\nFile exists only locally");
		open(prefixed_path, O_RDWR | O_CREAT, S_IRWXU);
	}
	// file only exists on server
	else if (local_ret == -1 && remote_ret != -1){
		printf("\nFile exists only in the server");
		grpc_afs_open(path);
	}
	else
		return -1;
	
	printf("In afs_open inside fuse client for %s\n",prefixed_path);
	return SUCCESS;
}

static int afs_read(const char *path, char *buf, size_t size, off_t offset,
					struct fuse_file_info *fi)
{
	int fd;
	int res;
	(void) fi;
	char prefixed_path[1024];
	char temp_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);
	strcpy(temp_path, prefixed_path);
	strcat(temp_path, ".tmp");
	
	// service read from temp file if it exists
	int tmp_fd = open(temp_path, O_RDONLY);
	if (tmp_fd != -1)
		fd = tmp_fd;
	else
		fd = open(prefixed_path, O_RDONLY);

	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);

	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int afs_write(const char *path, const char *buf, size_t size,
					 off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	(void) fi;
	char prefixed_path[1024];

	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	printf("\nThe file opened for writing is: %s\n",prefixed_path);
	printf("\nBuff: %s\nsize: %d\noffset: %d\n",buf,(int)size,(int)offset);

	fd = open(prefixed_path, fi->flags, S_IRWXU);
	if (fd == -1){
		printf("\nUnable to open the file");
		return -errno;
	}
	res = pwrite(fd, buf, size, offset);
	if (res == -1){
		printf("\nUnable to write to the file");
		res = -errno;
	}
	close(fd);
	return res;
}

static int afs_flush(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	// upload file to server always (maybe check for O_RDONLY) 
	/*if ((fi->flags & O_RDONLY) == O_RDONLY){
	  printf("\nReturning from flush without grpc");
	  return res;
	  }*/
	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);
	char temp_path[1024];
	strcpy(temp_path, prefixed_path);
	strcat(temp_path, ".tmp");

	if (open(temp_path, O_RDONLY) == -1)
		return -1;

	rename(temp_path, prefixed_path);

	printf("\nCalling grpc flush on file: %s\n",path);

	res = grpc_afs_flush(path);
	return res;
}

static int afs_rmdir(const char *path)
{
	int res = 0;

	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	res = rmdir(prefixed_path);
	res = grpc_afs_rmdir(path);
	return res;
}

static int afs_mkdir(const char *path, mode_t mode)
{
	int res = 0;

	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	res = mkdir(prefixed_path, mode);
	res = grpc_afs_mkdir(path);
	return res;
}

int afs_mknod(const char *path, mode_t mode, dev_t rdev) 
{
	int res;
	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	if (S_ISREG(mode)) {
		res = open(prefixed_path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} 
	else
		res = mknod(prefixed_path, mode, rdev);
	if (res == -1)
		return -errno;
	return 0;
}

static int afs_unlink(const char *path) 
{
	int res;
	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	res = unlink(prefixed_path);
	res = grpc_afs_unlink(path);
	if (res == -1)
		return -errno;
	return 0;
}

static int afs_rename(const char *from, const char *to) 
{
	int res;
	char prefixed_path[1024];

	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, from);

	char prefixed_to[1024];

	strcpy(prefixed_to, cache_path);
	strcat(prefixed_to, to);

	res = rename(prefixed_path, prefixed_to);
	res = grpc_afs_rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int afs_chmod(const char *path, mode_t mode) 
{
	int res;
	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	res = chmod(prefixed_path, mode);
	if (res == -1)
		return -errno;
	return 0;
}

static int afs_chown(const char *path, uid_t uid, gid_t gid) 
{
	int res;
	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	res = lchown(prefixed_path, uid, gid);
	if (res == -1)
		return -errno;
	return 0;
}

static int afs_truncate(const char *path, off_t size) 
{
	int res;
	char prefixed_path[1024];
	strcpy(prefixed_path, cache_path);
	strcat(prefixed_path, path);

	res = truncate(prefixed_path, size);
	if (res == -1)
		return -errno;
	return 0;
}

static int afs_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;
	return 0;
}

// recursive directory building function
static void _mkdir(const char *dir) {
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp),"%s",dir);
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
		if(*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	mkdir(tmp, S_IRWXU);
}

// FUSE operations table
static struct fuse_operations afs_oper = {
	.getattr = afs_getattr,
	.readdir = afs_readdir,
	.open = afs_open,
	.read = afs_read,
	.write = afs_write,
	.flush = afs_flush,
	.mkdir = afs_mkdir,
	.rmdir = afs_rmdir,
	.mknod= afs_mknod,
	.unlink= afs_unlink,
	.rename= afs_rename,
	.chmod= afs_chmod,
	.chown= afs_chown,
	.truncate = afs_truncate,
	.utimens = afs_utimens,
};

int main(int argc, char *argv[]) {

	if(argc < 3){
		printf("Usage: ./executable <cache-path> <mount-point> <optional-parameters>\n");
		return 0;
	}

	char *fuse_parameters[10];
	int i;

	strcpy(cache_path,argv[1]);
	fuse_parameters[0] = argv[0];

	for(i=2;i<argc;i++){
		// Skip for cache path
		if(i == 1)
			continue;
		fuse_parameters[i-1] = argv[i];
	}

	for(i=0; cache_path[i] != '\0'; i++){}
	if(cache_path[i-1] == '/'){
		cache_path[i-1] = '\0';
		//cache_path[i+1] = '\0';
	}
	printf("\nCache path is: %s\n", cache_path);

	umask(0);
	return fuse_main(argc-1, fuse_parameters, &afs_oper, NULL);
}
