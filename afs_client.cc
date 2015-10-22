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

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using afs::Request;
using afs::Reply;
using afs::AFS;

class AFSClient {
 public:
  AFSClient(std::shared_ptr<Channel> channel)
      : stub_(AFS::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string afs_open(const std::string& path) {
    // Data we are sending to the server.
    Request request;
    request.set_name(path);

    // Container for the data we expect from the server.
    Reply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    //Local AFS cache file handling
		std::ofstream file_stream;
    file_stream.open(path);

    // The actual RPC.
//    Status status = stub_->afs_open(&context, request, &reply);
    std::unique_ptr<ClientReader<Reply>> reader(stub_->afs_open(&context, request));
    while (reader->Read(&reply)) {
      //TODO think about handling metadata(maybe perform a separate rpc call)
      //save the file locally
      //file_stream << reply.data();
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

 private:
  std::unique_ptr<AFS::Stub> stub_;
};


extern "C" int grpc_afs_open(const char *path)
{
  AFSClient client(
      grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
  //std::string path("world");
  std::string r = client.afs_open(path);
  std::cout << r << std::endl;
  return 0;
}
