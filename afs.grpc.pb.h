// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: afs.proto
#ifndef GRPC_afs_2eproto__INCLUDED
#define GRPC_afs_2eproto__INCLUDED

#include "afs.pb.h"

#include <grpc++/support/async_stream.h>
#include <grpc++/impl/rpc_method.h>
#include <grpc++/impl/proto_utils.h>
#include <grpc++/impl/service_type.h>
#include <grpc++/support/async_unary_call.h>
#include <grpc++/support/status.h>
#include <grpc++/support/stub_options.h>
#include <grpc++/support/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class RpcService;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace afs {

class AFS GRPC_FINAL {
 public:
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    std::unique_ptr< ::grpc::ClientReaderInterface< ::afs::Reply>> afs_open(::grpc::ClientContext* context, const ::afs::Request& request) {
      return std::unique_ptr< ::grpc::ClientReaderInterface< ::afs::Reply>>(afs_openRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::afs::Reply>> Asyncafs_open(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::afs::Reply>>(Asyncafs_openRaw(context, request, cq, tag));
    }
    virtual ::grpc::Status afs_getattr(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::Stat* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::Stat>> Asyncafs_getattr(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::Stat>>(Asyncafs_getattrRaw(context, request, cq));
    }
    virtual ::grpc::Status afs_readdir(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::DirentReply* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::DirentReply>> Asyncafs_readdir(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::DirentReply>>(Asyncafs_readdirRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientWriterInterface< ::afs::FlushRequest>> afs_flush(::grpc::ClientContext* context, ::afs::FlushReply* response) {
      return std::unique_ptr< ::grpc::ClientWriterInterface< ::afs::FlushRequest>>(afs_flushRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::afs::FlushRequest>> Asyncafs_flush(::grpc::ClientContext* context, ::afs::FlushReply* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::afs::FlushRequest>>(Asyncafs_flushRaw(context, response, cq, tag));
    }
    virtual ::grpc::Status afs_rmdir(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::StatusReply* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::StatusReply>> Asyncafs_rmdir(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::StatusReply>>(Asyncafs_rmdirRaw(context, request, cq));
    }
    virtual ::grpc::Status afs_mkdir(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::StatusReply* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::StatusReply>> Asyncafs_mkdir(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::afs::StatusReply>>(Asyncafs_mkdirRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientReaderInterface< ::afs::Reply>* afs_openRaw(::grpc::ClientContext* context, const ::afs::Request& request) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::afs::Reply>* Asyncafs_openRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::afs::Stat>* Asyncafs_getattrRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::afs::DirentReply>* Asyncafs_readdirRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientWriterInterface< ::afs::FlushRequest>* afs_flushRaw(::grpc::ClientContext* context, ::afs::FlushReply* response) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::afs::FlushRequest>* Asyncafs_flushRaw(::grpc::ClientContext* context, ::afs::FlushReply* response, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::afs::StatusReply>* Asyncafs_rmdirRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::afs::StatusReply>* Asyncafs_mkdirRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub GRPC_FINAL : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::Channel>& channel);
    std::unique_ptr< ::grpc::ClientReader< ::afs::Reply>> afs_open(::grpc::ClientContext* context, const ::afs::Request& request) {
      return std::unique_ptr< ::grpc::ClientReader< ::afs::Reply>>(afs_openRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::afs::Reply>> Asyncafs_open(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::afs::Reply>>(Asyncafs_openRaw(context, request, cq, tag));
    }
    ::grpc::Status afs_getattr(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::Stat* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::Stat>> Asyncafs_getattr(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::Stat>>(Asyncafs_getattrRaw(context, request, cq));
    }
    ::grpc::Status afs_readdir(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::DirentReply* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::DirentReply>> Asyncafs_readdir(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::DirentReply>>(Asyncafs_readdirRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientWriter< ::afs::FlushRequest>> afs_flush(::grpc::ClientContext* context, ::afs::FlushReply* response) {
      return std::unique_ptr< ::grpc::ClientWriter< ::afs::FlushRequest>>(afs_flushRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::afs::FlushRequest>> Asyncafs_flush(::grpc::ClientContext* context, ::afs::FlushReply* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::afs::FlushRequest>>(Asyncafs_flushRaw(context, response, cq, tag));
    }
    ::grpc::Status afs_rmdir(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::StatusReply* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::StatusReply>> Asyncafs_rmdir(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::StatusReply>>(Asyncafs_rmdirRaw(context, request, cq));
    }
    ::grpc::Status afs_mkdir(::grpc::ClientContext* context, const ::afs::Request& request, ::afs::StatusReply* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::StatusReply>> Asyncafs_mkdir(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::afs::StatusReply>>(Asyncafs_mkdirRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::Channel> channel_;
    ::grpc::ClientReader< ::afs::Reply>* afs_openRaw(::grpc::ClientContext* context, const ::afs::Request& request) GRPC_OVERRIDE;
    ::grpc::ClientAsyncReader< ::afs::Reply>* Asyncafs_openRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq, void* tag) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::afs::Stat>* Asyncafs_getattrRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::afs::DirentReply>* Asyncafs_readdirRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientWriter< ::afs::FlushRequest>* afs_flushRaw(::grpc::ClientContext* context, ::afs::FlushReply* response) GRPC_OVERRIDE;
    ::grpc::ClientAsyncWriter< ::afs::FlushRequest>* Asyncafs_flushRaw(::grpc::ClientContext* context, ::afs::FlushReply* response, ::grpc::CompletionQueue* cq, void* tag) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::afs::StatusReply>* Asyncafs_rmdirRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::afs::StatusReply>* Asyncafs_mkdirRaw(::grpc::ClientContext* context, const ::afs::Request& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    const ::grpc::RpcMethod rpcmethod_afs_open_;
    const ::grpc::RpcMethod rpcmethod_afs_getattr_;
    const ::grpc::RpcMethod rpcmethod_afs_readdir_;
    const ::grpc::RpcMethod rpcmethod_afs_flush_;
    const ::grpc::RpcMethod rpcmethod_afs_rmdir_;
    const ::grpc::RpcMethod rpcmethod_afs_mkdir_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::Channel>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::SynchronousService {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status afs_open(::grpc::ServerContext* context, const ::afs::Request* request, ::grpc::ServerWriter< ::afs::Reply>* writer);
    virtual ::grpc::Status afs_getattr(::grpc::ServerContext* context, const ::afs::Request* request, ::afs::Stat* response);
    virtual ::grpc::Status afs_readdir(::grpc::ServerContext* context, const ::afs::Request* request, ::afs::DirentReply* response);
    virtual ::grpc::Status afs_flush(::grpc::ServerContext* context, ::grpc::ServerReader< ::afs::FlushRequest>* reader, ::afs::FlushReply* response);
    virtual ::grpc::Status afs_rmdir(::grpc::ServerContext* context, const ::afs::Request* request, ::afs::StatusReply* response);
    virtual ::grpc::Status afs_mkdir(::grpc::ServerContext* context, const ::afs::Request* request, ::afs::StatusReply* response);
    ::grpc::RpcService* service() GRPC_OVERRIDE GRPC_FINAL;
   private:
    std::unique_ptr< ::grpc::RpcService> service_;
  };
  class AsyncService GRPC_FINAL : public ::grpc::AsynchronousService {
   public:
    explicit AsyncService();
    ~AsyncService() {};
    void Requestafs_open(::grpc::ServerContext* context, ::afs::Request* request, ::grpc::ServerAsyncWriter< ::afs::Reply>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void Requestafs_getattr(::grpc::ServerContext* context, ::afs::Request* request, ::grpc::ServerAsyncResponseWriter< ::afs::Stat>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void Requestafs_readdir(::grpc::ServerContext* context, ::afs::Request* request, ::grpc::ServerAsyncResponseWriter< ::afs::DirentReply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void Requestafs_flush(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::afs::FlushReply, ::afs::FlushRequest>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void Requestafs_rmdir(::grpc::ServerContext* context, ::afs::Request* request, ::grpc::ServerAsyncResponseWriter< ::afs::StatusReply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void Requestafs_mkdir(::grpc::ServerContext* context, ::afs::Request* request, ::grpc::ServerAsyncResponseWriter< ::afs::StatusReply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
  };
};

}  // namespace afs


#endif  // GRPC_afs_2eproto__INCLUDED
