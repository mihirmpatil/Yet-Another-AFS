#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "grpc_afs_wrapper.h"

#define SUCCESS 0

//static const char CLIENT_DIR_PATH[1024];

static int afs_open(const char *path, struct fuse_file_info *fi)
{
  // check if local file exists
  // check if the local file is the updated version as the server
  // if not exists or not updated make grpc call
  grpc_afs_open(path);
  // save file locally - handle this in grpc client itself to avoid passing data here
  return SUCCESS;
}



// FUSE operations table
static struct fuse_operations afs_oper = {
  .open = afs_open,
};

int main(int argc, char *argv[]) {
  return fuse_main(argc, argv, &afs_oper, NULL);
}
