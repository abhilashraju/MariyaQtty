/**Copyright[2021][abhilash raju]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
**/
#include <server.hpp>
#include <iostream>
#include <fstream>
#include <MqttServerConfig.h>
int
main(int argc, char* argv[])
{
    // Check command line arguments.
    using namespace mariaqtty;
    if (argc != 4)
    {
        std::cerr <<
            "Usage:Server <address> <port> <doc_root>\n" <<
            "Example:\n" <<
            "    Server 0.0.0.0 8080 .\n";
        return EXIT_FAILURE;
    }
    auto address = net::ip::make_address(argv[1]);
    auto port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto doc_root = argv[3];

    Server serv(Configurtion{ address,port,doc_root});
    serv.state()->router().register_http_handler("/hello",[](Server::Router::request& req, Server::Router::string_view doc_root){
        return Server::Router::http_resp{ http::status::ok,"text/html","<html><body><h1>Hello World!</h1><p>some content...</p></body></html>"};
    });
    serv.state()->router().register_http_handler("/", [](Server::Router::request& req, Server::Router::string_view doc_root) {
        std::ifstream stream;
        stream.open("C:/Users/rabhil/work/CppCon2018/chat_client.html");
        std::stringstream strstream;
        strstream<< stream.rdbuf();
        return Server::Router::http_resp{ http::status::ok,"text/html", strstream.str()};
    });

    serv.state()->router().register_ws_handler(": hello",[]( Server::Router::string_view data){
        return Server::Router::ws_resp{ Server::Router::BroadCast,"From Server " + std::string(data.data(),data.length()) };
    });

    serv.state()->router().register_ws_handler(": hi", [](Server::Router::string_view data) {
        return Server::Router::ws_resp{ Server::Router::Peer,"From Server "+std::string(data.data(),data.length()) };
        });

    serv.state()->router().register_ws_handler([](auto data) { return (data.starts_with(": bye")); }, [](Server::Router::string_view data) {
        return Server::Router::ws_resp{ Server::Router::BroadCast,"From Server " + std::string(data.data(),data.length()) };
        });
    serv.start();

  
    return EXIT_SUCCESS;
}
