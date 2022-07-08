/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "../build/simpleCalculation.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerCredentials;
using simpleCalculation::SimCalc;
using simpleCalculation::CalcRequest;
using simpleCalculation::CalcReply;
using simpleCalculation::opr;

class CalcServerImpl final: public SimCalc::Service {
public:
    Status DoCalc(ServerContext* context, const CalcRequest* req, CalcReply* reply) override {
        int32_t result;
        result = req->first() + req->second();
        reply->set_result(result);
        return Status::OK;
    }
};
// Logic and data behind the server's behavior.

void RunServer() {
    std::string server_string = "127.0.0.1:10001";
    CalcServerImpl service;
    ServerBuilder builder;
    std::unique_ptr<Server> server = builder
            .AddListeningPort(server_string,grpc::InsecureServerCredentials())
            .RegisterService(&service)
            .BuildAndStart();

  // Finally assemble the server.
  std::cout << "Server listening on " << server_string << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
