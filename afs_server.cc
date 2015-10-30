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

#define BUF_LEN 1024

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;
using afs::Request;
using afs::Reply;
using afs::DirentReply;
using afs::Dirent;
using afs::Stat;
using afs::FlushRequest;
using afs::FlushReply;
using afs::AFS;

// TODO add code to handle path translations


// Logic and data behind the server's behavior.
class AFSServiceImpl final : public AFS::Service {
  Status afs_open(ServerContext* context, const Request* request,
                 ServerWriter<Reply>* writer) override {
    
		
		Reply reply;
		std::string path = request->name();
		std::cout << "in afs_open for " << path << std::endl;
		
		std::ifstream file_stream;
		//std::string& buffer;
		char *buffer = new char [BUF_LEN];
		file_stream.open(path);
		while (!file_stream.eof()) {
			file_stream.read(buffer, BUF_LEN);
			std::string data = buffer;
			reply.set_data(data);
			writer->Write(reply);
		}
		file_stream.close();
		
    return Status::OK;
  }

	Status afs_readdir(ServerContext* context, const Request* request,
									DirentReply* response) override {
		
		std::string path = request->name();
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
			dirent->set_reclen(de->d_reclen);
			dirent->set_d_type(de->d_type);
			count++;

		}
		response->set_count(count);
		closedir(dp);
		
		return Status::OK;
	}

	Status afs_getattr(ServerContext *context, const Request* request, 
									Stat *response) override {
		
		std::string path = request->name();
		//path = "/tmp/server/random_file";
		std::cout<<"Requested path:"<<path<<"\n";

		struct stat *st = (struct stat*)malloc(sizeof(struct stat));
		int res = lstat(path.c_str(), st);
		//if (res == -1)
		//	return Status::NOT_FOUND;
		
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
		
		return Status::OK;
	}

	Status afs_flush(ServerContext *context, ServerReader<FlushRequest> *reader, 
										FlushReply *response) override {

		FlushRequest request;

		reader->Read(&request);
		std::string path = request.path();
		std::ofstream file_stream;
		std::cout<<"\nOpening file to write: "<<path;
		file_stream.open(path);
		std::string file_data;

		while (reader->Read(&request)) {
			file_data = request.data();
			std::cout<<"\nWriting data:"<<file_data;
			file_stream << file_data;
		}
		
		std::cout<<"\nFinished writing the file";

		file_stream.close();
		response->set_status(0);
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

int main(int argc, char** argv) {
	// TODO take path as input from command line and monitor that path
  RunServer();

  return 0;
}
