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

//static const char CLIENT_DIR_PATH[1024];


// TODO implement this function
static int afs_getattr(const char *path, struct stat *stbuf)
{
	int res;

	res = lstat(path, stbuf);
	if (res == -1) 
		return -errno;

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
	printf("In afs_open inside fuse client\n");
  grpc_afs_open(path);
  // save file locally - handle this in grpc client itself to avoid passing data here
  return SUCCESS;
}



// FUSE operations table
static struct fuse_operations afs_oper = {
  .getattr = afs_getattr,
	.readdir = afs_readdir,
	.open = afs_open,
};

int main(int argc, char *argv[]) {
	//grpc_afs_open("random_file");
  return fuse_main(argc, argv, &afs_oper, NULL);
}
