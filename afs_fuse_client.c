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


char prefixed_path[1024];

// TODO implement this function
static int afs_getattr(const char *path, struct stat *stbuf)
{
	int res;

	//res = lstat(path, stbuf);
	//if (res == -1) 
	struct stat *s = grpc_afs_getattr(path, stbuf);
	
	stbuf->st_dev = s->st_dev;
	stbuf->st_ino = s->st_ino;
	stbuf->st_mode = s->st_mode;
	stbuf->st_nlink = s->st_nlink;
	stbuf->st_uid = s->st_uid;
	stbuf->st_gid = s->st_gid;
	stbuf->st_rdev = s->st_rdev;
	stbuf->st_size = s->st_size;
	stbuf->st_atime = s->st_atime;
	stbuf->st_mtime = s->st_mtime;
	stbuf->st_ctime = s->st_ctime;
	stbuf->st_blksize = s->st_blksize;
	stbuf->st_blocks = s->st_blocks;

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
	// TODO push file to server here

	/*char prefixed_path[1024];
	strcpy(prefixed_path, "/tmp/cache");
	strcat(prefixed_path, path);*/
	
	printf("\nCalling grpc flush on file: %s\n",path);

	res = grpc_afs_flush(path);
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
};

int main(int argc, char *argv[]) {
	//grpc_afs_open("random_file");
  return fuse_main(argc, argv, &afs_oper, NULL);
}
