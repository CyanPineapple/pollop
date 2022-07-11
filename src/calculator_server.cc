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
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>

#include "../build/simpleCalculation.grpc.pb.h"
#include "../build/healthCheck_hb.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerCredentials;

namespace pservice = pollop::service;
namespace phealth = pollop::health;

using pservice::simpleCalculation::SimCalc;
using pservice::simpleCalculation::CalcRequest;
using pservice::simpleCalculation::CalcReply;
using pservice::simpleCalculation::opr;

using phealth::hb::HeartBeat;
using phealth::hb::HeartbeatRequest;
using phealth::hb::HeartbeatResponse;

typedef std::string service_name;

class CalcServerImpl final: public SimCalc::Service {
public:
    Status DoCalc(ServerContext* context, const CalcRequest* req, CalcReply* reply) override {
        int32_t result;
		switch (req->opera()) {
			case 0:
				result = req->first() + req->second();
				break;
			case 1:
				result = req->first() - req->second();
				break;
			case 2:
				result = req->first() * req->second();
				break;
			case 3:
				result = req->first() / req->second();
				break;
		}
        reply->set_result(result);
        return Status::OK;
    }
};

class HeartBeatImpl final: public HeartBeat::Service {
public:

   std::unordered_map<service_name , HeartbeatResponse::ServingStatus> health_table = {
           {"", HeartbeatResponse::ServingStatus::HeartbeatResponse_ServingStatus_SERVING},
           {"simpleCalculation", HeartbeatResponse::ServingStatus::HeartbeatResponse_ServingStatus_NOT_SERVING},
   };

    Status Check(ServerContext* context, const HeartbeatRequest* req, HeartbeatResponse* reply) override {
        HeartbeatResponse::ServingStatus rep_status = req->service().empty() ? health_table[""] : health_table[req->service()];
        reply->set_status(rep_status);
        return Status::OK;
    }
    Status Watch(ServerContext* context, const HeartbeatRequest* req, grpc::ServerWriter<HeartbeatResponse>* writer) override {
        HeartbeatResponse::ServingStatus rep_status = req->service().empty() ? health_table[""] : health_table[req->service()];
        HeartbeatResponse resp;
        resp.set_status(rep_status);
       for (int i = 0; i < 5; i++) {
           std::this_thread::sleep_for(std::chrono::seconds(1)) ;
           writer->Write(resp);
       }

       return Status::OK;
    }


};
// Logic and data behind the server's behavior.

void RunServer() {
    std::string server_string = "localhost:10001";
    CalcServerImpl calc_service;
    HeartBeatImpl health_service;
    ServerBuilder builder;
    std::unique_ptr<Server> server = builder
            .AddListeningPort(server_string,grpc::InsecureServerCredentials())
            .RegisterService(&calc_service)
            .RegisterService(&health_service)
            .BuildAndStart();

  // Finally assemble the server.
  std::cout << "Worker is on! Currently listening on " << server_string << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
