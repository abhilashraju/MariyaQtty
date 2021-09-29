
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
#include "net.hpp"
#include <memory>
#include <string>
#include "shared_state.hpp"
namespace mariaqtty{


// Forward declaration
//class shared_state;

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    public:
    using Router = shared_state::Router;
    private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::shared_ptr<shared_state> state_;

    void fail(error_code ec, char const* what);
    void on_accept(error_code ec);

public:
    listener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        std::shared_ptr<shared_state> const& state);

    // Start accepting incoming connections
    void run();
    auto state(){return state_;}
};
}



