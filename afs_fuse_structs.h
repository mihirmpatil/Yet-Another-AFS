struct afs_dirent {
	char name[256];
	int reclen;
	int d_type;
};

struct afs_dirent_array{
  struct afs_dirent *dirent_arr;
  int count;
};
