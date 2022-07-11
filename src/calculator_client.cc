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

#include <grpcpp/grpcpp.h>

#include "../build/simpleCalculation.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
namespace pservice = pollop::service;
using pservice::simpleCalculation::SimCalc;
using pservice::simpleCalculation::CalcRequest;
using pservice::simpleCalculation::CalcReply;
using pservice::simpleCalculation::opr;


class CalClient {
public:
    CalClient (std::shared_ptr<Channel> channel)
    :  stub_(SimCalc::NewStub(channel)) {};

    int32_t make_calculation(int32_t first, int32_t second, opr operand) {
        grpc::ClientContext context;
        CalcRequest request;
        CalcReply reply;

        request.set_first(first);
        request.set_opera(operand);
        request.set_second(second);
        Status status = stub_->DoCalc(&context,request, &reply);
        if (status.ok()) return reply.result();
        else return -1;
    };
private:
    std::unique_ptr<SimCalc::Stub> stub_ ;
};


int main() {
    std::string server_string = "127.0.0.1:10001";

    std::shared_ptr<Channel> channel = grpc::CreateChannel(server_string, grpc::InsecureChannelCredentials());
    CalClient client(channel);
    int32_t res = client.make_calculation(3,2, opr::MUL);
    std::cout << "The result is: "<< res << std::endl;
    return 0;
}
/***
class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target="
                  << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
***/
