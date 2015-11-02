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
/*
	struct stat *remote_stat = (struct stat *)malloc(sizeof(struct stat));
	int remote_return = grpc_afs_getattr(path, &stbuf, cache_path);
	int local_return = 0;//lstat(prefixed_path, stbuf);
	if (remote_return == -1 && local_return == -1)
		return -errno;
*/
	if(grpc_afs_getattr(path, &stbuf, cache_path) == -1) {
		if(lstat(prefixed_path, stbuf) == -1){
			//lstat(prefixed_path, stbuf);
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

/*	dp = opendir(prefixed_path);
	if (dp != NULL) {

		while ((de = readdir(dp)) != NULL) {
			struct stat st;
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;
			if (filler(buf, de->d_name, &st, 0))
				break;
		}
	}
	else {*/
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
	//}

	//closedir(dp);
	return 0;
}


static int afs_open(const char *path, struct fuse_file_info *fi)
{
	// check if local file exists
	// check if the local file is the updated version as the server - use grpc_afs_getattr
	// if not exists or not updated make grpc call
	printf("In afs_open inside fuse client for %s\n",path);
	grpc_afs_open(path);
	return SUCCESS;
}

static int afs_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	int fd;
	int res;
	(void) fi;
	char prefixed_path[1024];
	strcpy(prefixed_path, "/tmp/cache");
	strcat(prefixed_path, path);
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
	strcpy(prefixed_path, "/tmp/cache");
	strcat(prefixed_path, path);

	printf("\nThe file opened for writing is: %s\n",prefixed_path);
	printf("\nBuff: %s\nsize: %d\noffset: %d\n",buf,(int)size,(int)offset);

	fd = open(prefixed_path, O_WRONLY);
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

	/*char prefixed_path[1024];
		strcpy(prefixed_path, "/tmp/cache");
		strcat(prefixed_path, path);*/

	printf("\nCalling grpc flush on file: %s\n",path);

	res = grpc_afs_flush(path);
	return res;
}

static int afs_rmdir(const char *path)
{
	int res = 0;

	char prefixed_path[1024];
	strcpy(prefixed_path, "/tmp/cache");
	strcat(prefixed_path, path);

	res = rmdir(prefixed_path);
	res = grpc_afs_rmdir(path);
	return res;
}

static int afs_mkdir(const char *path, mode_t mode)
{
	int res = 0;

	char prefixed_path[1024];
	strcpy(prefixed_path, "/tmp/cache");
	strcat(prefixed_path, path);

	res = mkdir(prefixed_path, mode);
	res = grpc_afs_mkdir(path);
	return res;
}

int afs_mknod(const char *path, mode_t mode, dev_t rdev) 
{
	int res;
	char prefixed_path[1024];
	strcpy(prefixed_path, "/tmp/cache");
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
	strcpy(prefixed_path, "/tmp/cache");
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
	strcpy(prefixed_path, "/tmp/cache");
	strcat(prefixed_path, from);
	char prefixed_to[1024];
	strcpy(prefixed_to, "/tmp/cache");
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
		strcpy(prefixed_path, "/tmp/cache");
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
		strcpy(prefixed_path, "/tmp/cache");
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
		strcpy(prefixed_path, "/tmp/cache");
		strcat(prefixed_path, path);

	res = truncate(prefixed_path, size);
	if (res == -1)
		return -errno;
	return 0;
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

	printf("\nCache path is: %s\n", cache_path);
	return fuse_main(argc-1, fuse_parameters, &afs_oper, NULL);
}
