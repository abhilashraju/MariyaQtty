#pragma once
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
#include "listener.hpp"
#include "shared_state.hpp"
#include <boost/asio/signal_set.hpp>
#include <type_traits>
namespace mariaqtty{
    using address = net::ip::address;
    struct Configurtion{
       
        
        address ip;
        unsigned short port{80};
        std::string doc_root;
    };
    class Server{
        public:
        using Router = listener::Router;
        private:
        Configurtion conf;
        net::io_context ioc;
        std::shared_ptr<listener> list;
        public:
        Server(Configurtion conf):conf(std::move(conf)){
             list=std::make_shared<listener>(
                ioc,
                tcp::endpoint{ conf.ip, conf.port},
                std::make_shared<shared_state>(conf.doc_root));
        }
        void start(){
            list->run();
            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait(
                [=](boost::system::error_code const&, int)
                {
                    // Stop the io_context. This will cause run()
                    // to return immediately, eventually destroying the
                    // io_context and any remaining handlers in it.
                    ioc.stop();
                });

            // Run the I/O service on the main thread
            ioc.run();
        }
        auto state(){return list->state();}
    };
}