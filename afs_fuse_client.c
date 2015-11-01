#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
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

	if(lstat(prefixed_path, stbuf) == -1){
		if(grpc_afs_getattr(path, &stbuf, cache_path) == -1){
			lstat(prefixed_path, stbuf);
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

	dp = opendir(path);
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
	else {
		struct afs_dirent *dirent_arr = grpc_afs_readdir(path);
		int count = sizeof(dirent_arr)/sizeof(struct afs_dirent);
		int i;
		for (i = 0; i < count; i++) {
			struct stat st;
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;
			if (filler(buf, de->d_name, &st, 0))
				break;
		}
	}

	closedir(dp);
	return 0;
}


static int afs_open(const char *path, struct fuse_file_info *fi)
{
	// check if local file exists
	// check if the local file is the updated version as the server
	// if not exists or not updated make grpc call
	printf("In afs_open inside fuse client for %s\n",path);
	grpc_afs_open(path);
	// save file locally - handle this in grpc client itself to avoid passing data here
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
	// upload file to server always 

	/*char prefixed_path[1024];
		strcpy(prefixed_path, "/tmp/cache");
		strcat(prefixed_path, path);*/

	printf("\nCalling grpc flush on file: %s\n",path);

	res = grpc_afs_flush(path);
	return res;
}

static int afs_rmdir(const char *path){
	int res = 0;

	res = grpc_afs_rmdir(path);
	return res;
}

static int afs_mkdir(const char *path, mode_t mode){
	int res = 0;

	res = grpc_afs_mkdir(path);
	return res;
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
};

int main(int argc, char *argv[]) {

	if(argc < 3){
		printf("Usage: ./executable <cache-path> <mount-point> <optional-parameters>");
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

	return fuse_main(argc-1, fuse_parameters, &afs_oper, NULL);
}
