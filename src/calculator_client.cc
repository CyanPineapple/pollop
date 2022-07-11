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
#include "../build/healthCheck_hb.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
namespace pservice = pollop::service;
using pservice::simpleCalculation::SimCalc;
using pservice::simpleCalculation::CalcRequest;
using pservice::simpleCalculation::CalcReply;
using pservice::simpleCalculation::opr;

namespace phealth = pollop::health;
using phealth::hb::HeartBeat;
using phealth::hb::HeartbeatRequest;
using phealth::hb::HeartbeatResponse;


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

class HealthClient {
public:
    HealthClient (std::shared_ptr<Channel> channel)
    :  stub_(HeartBeat::NewStub(channel)) {};

    int32_t heartbeat_check (std::string service_name) {
        grpc::ClientContext context;
        HeartbeatRequest request;
        HeartbeatResponse res;

        request.set_service(service_name);

        Status status = stub_->Check(&context, request, &res);
        if (status.ok()) {
            std::cout << "Heartbeat received: "<< res.status() << std::endl;
            return 1;
        }
        else return -1;
    };

    void heartbeat_watch (std::string service_name) {
        grpc::ClientContext context;
        HeartbeatRequest request;
        request.set_service(service_name);

        std::unique_ptr<grpc::ClientReader<HeartbeatResponse>> reader = stub_->Watch(&context,request);
        HeartbeatResponse msg;
        std::cout << "Start Watch Heartbeat!" << std::endl;
        while (reader->Read(&msg)) {
            std::cout << "HeartBeat: " << msg.status() << std::endl;
        }
    }

private:
   std::unique_ptr<HeartBeat::Stub> stub_ ;
};

int main() {
    grpc::ChannelArguments args;
    args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 5000);
    args.SetLoadBalancingPolicyName("round_robin");
    std::shared_ptr<Channel> channel = grpc::CreateCustomChannel("dns:localhost:10001",grpc::InsecureChannelCredentials(),args);
    //std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:10002",grpc::InsecureChannelCredentials());

    HealthClient health_client(channel);
    CalClient cal_client(channel);

    health_client.heartbeat_watch("simpleCalculation");
    //int32_t resul = cal_client.make_calculation(2,3,opr::ADD);
    //std::cout << resul << std::endl;

    return 0;
}