/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include <sys/stat.h>
#include <grpc++/grpc++.h>

#include "afs.grpc.pb.h"
#include "afs_fuse_structs.h"

#define BUF_LEN 1024
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientWriter;
using grpc::Status;
using grpc::StatusCode;
using afs::StatusReply;
using afs::Request;
using afs::Reply;
using afs::Dirent;
using afs::DirentReply;
using afs::Stat;
using afs::FlushRequest;
using afs::RenameRequest;
using afs::FlushReply;
using afs::AFS;

class AFSClient {
public:
	std::string cache_path = "/tmp/cache";
	AFSClient(std::shared_ptr<Channel> channel)
		: stub_(AFS::NewStub(channel)) {}

	std::string afs_open(const std::string& path) {
		
		Request request;
		Reply reply;

		// Data we are sending to the server.
		request.set_name(path);		

		// Context for the client
		ClientContext context;

		//Local AFS cache file handling
		std::ofstream file_stream;
		file_stream.open(cache_path + path + ".tmp");

		// The actual RPC.
		std::cout << "The actual GRPC for " << cache_path + path << std::endl;
		// Status status = stub_->afs_open(&context, request, &reply);
		std::unique_ptr<ClientReader<Reply>> reader(stub_->afs_open(&context, request));
		while (reader->Read(&reply)) {
			//save the file locally
			file_stream << reply.data();
			std::cout << reply.data() << std::endl;
		}

		Status status = reader->Finish();
		file_stream.close();

		// Act upon its status.
		if (status.ok()) {
			return reply.data();
		} else {
			return "RPC failed";
		}
	}

	//Think about the return value here
	struct afs_dirent_array afs_readdir(const std::string& path) {

		Request request;
		request.set_name(path);
		DirentReply dirent_reply;
		ClientContext context;
		struct afs_dirent *dirent_arr;
		struct afs_dirent_array dirent_array_st;
		
		//std::cout<<"\nDoing grpc readdir for path: "<<path<<std::endl;
		Status status = stub_->afs_readdir(&context, request, &dirent_reply);
		int dirent_count = dirent_reply.count();
		
		//std::cout<<"\nreply count: "<<dirent_count<<std::endl;
		dirent_arr = (struct afs_dirent*)malloc(dirent_count*sizeof(struct afs_dirent));
		
		for (int i = 0; i < dirent_count; i++) {
			strcpy(dirent_arr[i].name, dirent_reply.mutable_dirent(i)->name().c_str()); 
			dirent_arr[i].reclen = dirent_reply.mutable_dirent(i)->reclen();
			dirent_arr[i].d_type = dirent_reply.mutable_dirent(i)->d_type();
		}
		
		dirent_array_st.dirent_arr = dirent_arr;
		dirent_array_st.count = dirent_count;
		
		return dirent_array_st;
	}
  
	int afs_getattr(const std::string& path, struct stat **st, const std::string& cache_path) {

		Request request;
		request.set_name(path);
		Stat s;
		ClientContext context;

		std::cout<<"\nDoing grpc call to getattr for path:"<<path;
		//return lstat(path.c_str(),*st);
		Status status = stub_->afs_getattr(&context, request, &s);

		(*st)->st_dev = s.dev();
		(*st)->st_ino = s.ino();
		(*st)->st_mode = s.mode();
		(*st)->st_nlink = s.nlink();
		(*st)->st_uid = s.uid();
		(*st)->st_gid = s.gid();
		(*st)->st_rdev = s.rdev();
		(*st)->st_size = s.size();
		(*st)->st_atime = s.a_time();
		(*st)->st_mtime = s.m_time();
		(*st)->st_ctime = s.c_time();
		(*st)->st_blksize = s.block_size();
		(*st)->st_blocks = s.blocks();

		if(status.error_code() == 1){ //StatusCode.CANCELLED
			std::cout<<"\nReturning -1 in getattr for path:"<<path;
			return -1;
		}

		std::cout<<"\nReturning 0 in getattr for path:"<<path;
		return 0;

	}

	int afs_flush(const std::string& path) {

		FlushRequest request;
		FlushReply response;
		ClientContext context;
		request.set_path(path);
		std::unique_ptr<ClientWriter<FlushRequest> > writer(
															stub_->afs_flush(&context, &response));

		// send path
		writer->Write(request);

		std::ifstream file_stream;
		file_stream.open(cache_path + path);
		std::cout<<"\nTrying to open:"<<path;
			
		while (!file_stream.eof()) {
			  
			char *buffer = new char[BUF_LEN];
			file_stream.read(buffer, BUF_LEN);
			std::string file_data = buffer;
			std::cout<<"\nWriting data: "<<file_data<<std::endl;
			std::cout<<"\nlength: "<<file_data.length()<<std::endl;
			request.set_data(file_data);

			std::cout<<"\nBefore sending to the server";
			writer->Write(request);			
			std::cout<<"After wrinting to the server";
			delete[] buffer;

			if((int)file_data.length() < 1024)
				break;
		}
		std::cout<<"\nTrying to close the file stream";
		file_stream.close();
		std::cout<<"\nOut of the while loop";
		writer->WritesDone();
		Status status = writer->Finish();

		return 0;
	}

	int afs_mkdir(const std::string& path) {
		Request request;
		StatusReply reply;

		request.set_name(path);
		ClientContext context;

		Status status = stub_->afs_mkdir(&context, request, &reply);
		return reply.status();
	}

	int afs_rmdir(const std::string& path) {
		Request request;
		StatusReply reply;

		request.set_name(path);
		ClientContext context;

		Status status = stub_->afs_rmdir(&context, request, &reply);
		return reply.status();
	}

	int afs_unlink(const std::string& path) {
		Request request;
		StatusReply reply;

		request.set_name(path);
		ClientContext context;

		Status status = stub_->afs_unlink(&context, request, &reply);
		return reply.status();
	}

	int afs_rename(const std::string& from, const std::string& to) {
		RenameRequest request;
		StatusReply reply;

		request.set_old_name(from);
		request.set_new_name(to);
		ClientContext context;

		Status status = stub_->afs_rename(&context, request, &reply);
		return reply.status();
	}

private:
	std::unique_ptr<AFS::Stub> stub_;
};


extern "C" int grpc_afs_open(const char *path) {
	std::cout << "In exported open implementation " << std::endl;
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	std::string r = client.afs_open(path);
	std::cout << r << std::endl;
	return 0;
}

extern "C" struct afs_dirent_array grpc_afs_readdir(const char *path) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	struct afs_dirent_array r = client.afs_readdir(path);
	return r;
}

extern "C" int grpc_afs_getattr(const char *path, struct stat **s, const char *cache_path) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	return client.afs_getattr(path, s, cache_path);
}

extern "C" int grpc_afs_flush(const char *path) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	int s = client.afs_flush(path);
	return s;
}

extern "C" int grpc_afs_mkdir(const char *path) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	int s = client.afs_mkdir(path);
	return s;
}

extern "C" int grpc_afs_rmdir(const char *path) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	int s = client.afs_rmdir(path);
	return s;
}

extern "C" int grpc_afs_unlink(const char *path) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	int s = client.afs_unlink(path);
	return s;
}

extern "C" int grpc_afs_rename(const char *from, const char *to) {
	AFSClient client(
					 grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
	int s = client.afs_rename(from, to);
	return s;
}
