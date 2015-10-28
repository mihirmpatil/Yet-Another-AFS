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
using grpc::Status;
using afs::Request;
using afs::Reply;
using afs::DirentReply;
using afs::Dirent;
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
