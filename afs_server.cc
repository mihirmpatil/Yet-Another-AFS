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

#include <grpc++/grpc++.h>

#include "afs.grpc.pb.h"
#include "dirent.h"
#include "sys/stat.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/file.h"

#define BUF_LEN 1024

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;
using afs::Request;
using afs::Reply;
using afs::StatusReply;
using afs::DirentReply;
using afs::Dirent;
using afs::Stat;
using afs::FlushRequest;
using afs::RenameRequest;
using afs::FlushReply;
using afs::AFS;

std::string afs_root;

// Logic and data behind the server's behavior.
class AFSServiceImpl final : public AFS::Service {
	Status afs_open(ServerContext* context, const Request* request,
			ServerWriter<Reply>* writer) override {


		Reply reply;
		int count = 0;
		std::cout << "in afs_open for " << request->name() << std::endl;
		std::ifstream file_stream;

		file_stream.open(afs_root + request->name());
		
		// wait if file is being written
		std::string lockfile = (afs_root + request->name() + ".lock");
		int fd = open(lockfile.c_str(), O_WRONLY | O_CREAT);
		flock(fd, LOCK_SH);

		file_stream.seekg(0, file_stream.end);
		int filesize = file_stream.tellg();
		file_stream.seekg(0, file_stream.beg);

		std::cout<<"\nFile size = "<<filesize<<std::endl;

		while (count < filesize){
			char *buffer;
			int currBufferLen = 0;

			if(count + BUF_LEN <= filesize){
				//std::cout<<"\nIn if"<<std::endl;
				currBufferLen = BUF_LEN;
			}
			else{
				//std::cout<<"\nIn else: s= "<<(filesize-count)<<std::endl;
				currBufferLen = filesize-count;
			}

			buffer = new char[currBufferLen+1];
			file_stream.read(buffer, currBufferLen);
			count += currBufferLen;

			std::string data = std::string(buffer);
			reply.set_data(data);
			reply.set_size(currBufferLen);
			//std::cout<<"\nData:\n"<<data;
			//std::cout<<"\ncount = "<<count;
			writer->Write(reply);
			
			delete[] buffer;
		}
		
		std::cout<<"\nLength = "<<count<<std::endl;
		
		file_stream.close();
		// unlock and cleanup
		flock(fd, LOCK_UN);
		close(fd);
		unlink(lockfile.c_str());

		return Status::OK;
	}

	Status afs_readdir(ServerContext* context, const Request* request,
			DirentReply* response) override {

		std::string path = afs_root + request->name();
		std::cout << "Server readdir() path: " << path << std::endl;
		DIR *dp;
		struct dirent *de;
		//(void) offset;
		//(void) fi;

		dp = opendir(path.c_str());
		//if (dp == NULL)
		//	return -errno;

		int count = 0;
		while ((de = readdir(dp)) != NULL) {
			struct stat st;
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;

			Dirent *dirent = response->add_dirent();
			dirent->set_name(de->d_name);
			dirent->set_reclen(de->d_ino); // Changed from de->d_reclen
			dirent->set_d_type(de->d_type);

			//std::cout<<"\nSending: name:"<<dirent->name()<<", ino:"<<dirent->reclen()<<", type:"<<dirent->d_type()<<std::endl;

			count++;

		}
		response->set_count(count);
		closedir(dp);

		return Status::OK;
	}

	Status afs_getattr(ServerContext *context, const Request* request, 
			Stat *response) override {

		std::string path = afs_root + request->name();
		//path = "/tmp/server/random_file";
		std::cout<<"Requested path:"<<path<<"\n";

		struct stat *st = (struct stat*)malloc(sizeof(struct stat));
		int res = lstat(path.c_str(), st);

		response->set_dev(st->st_dev);
		response->set_ino(st->st_ino);
		response->set_mode(st->st_mode);
		response->set_nlink(st->st_nlink);
		response->set_uid(st->st_uid);
		response->set_gid(st->st_gid);
		response->set_rdev(st->st_rdev);
		response->set_size(st->st_size);
		response->set_a_time(st->st_atime);
		response->set_m_time(st->st_mtime);
		response->set_c_time(st->st_ctime);
		response->set_block_size(st->st_blksize);
		response->set_blocks(st->st_blocks);

		if (res == -1){
			return Status::CANCELLED;
		}
		/*else
			response->set_success(0);*/

		return Status::OK;
	}

	Status afs_flush(ServerContext *context, ServerReader<FlushRequest> *reader, 
			FlushReply *response) override {

		FlushRequest request;

		reader->Read(&request);

		std::string path = afs_root + request.path();
		std::string lockfile = path + ".lock";
		int fd = open(lockfile.c_str(), O_WRONLY | O_CREAT);
		flock(fd, LOCK_EX);
	
		std::ofstream file_stream;
		std::string file_data;

		std::cout<<"\nOpening file to write: "<<path;
		file_stream.open(path);


		while (reader->Read(&request)) {
			file_data = request.data().substr(0,request.size());
			//std::cout<<"\nSize:"<<request.size()<<"\n";
			//std::cout<<"\nData:"<<request.data()<<"\n";
			file_stream << file_data;
		}

		std::cout<<"\nFinished writing the file";

		file_stream.close();
		// release lock and clean up
		flock(fd, LOCK_UN);
		close(fd);
		unlink(lockfile.c_str());

		response->set_status(0);
		return Status::OK;
	}

	Status afs_rmdir(ServerContext *context, const Request* request, StatusReply* response){
		struct stat dirstat;
		std::string path = afs_root + request->name();
		int res = lstat(path.c_str(), &dirstat);

		if(res == -1){
			response->set_status(-2);
		}
		else{
			response->set_status(rmdir(path.c_str()));
		}

		return Status::OK;
	}

	Status afs_mkdir(ServerContext *context, const Request* request, StatusReply* response){

		std::cout<<"\nRequested to make path: "<<request->name();
		int ret = mkdir((afs_root+request->name()).c_str(),S_IRWXO | S_IRWXU);
		response->set_status(ret);
		return Status::OK;

	}

	Status afs_unlink(ServerContext *context, const Request* request, StatusReply* response){

		int ret = unlink((afs_root + request->name()).c_str());
		response->set_status(ret);
		return Status::OK;
	}

	Status afs_rename(ServerContext *context, const RenameRequest* request, StatusReply* response){
//		char *from = (afs_root + request->old_name()).c_str();
//		char *to = (afs_root + request->new_name()).c_str();
		int ret = rename((afs_root + request->old_name()).c_str(), (afs_root + request->new_name()).c_str());
		response->set_status(ret);
		return Status::OK;
	}
};

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	AFSServiceImpl service;

	ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main(int argc, char* argv[]) {
	if(argc != 2){
		std::cout<<"Usage: ./<executable> <afs_root_path>" << std::endl;
		return 0;
	}

	afs_root = argv[1];

	if(afs_root[afs_root.length()-1] == '/'){
		afs_root = afs_root.substr(0,afs_root.length()-1);
		//afs_root = "";
	}

	RunServer();

	return 0;
}
