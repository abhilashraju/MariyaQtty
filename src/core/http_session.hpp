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
#include "beast.hpp"
#include "shared_state.hpp"
#include <cstdlib>

#include <memory>

/** Represents an established HTTP connection
*/
namespace mariaqtty{
class http_session : public std::enable_shared_from_this<http_session>
{
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    std::shared_ptr<shared_state> state_;
    http::request<http::string_body> req_;
    
    void fail(error_code ec, char const* what);
    void on_read(error_code ec, std::size_t);
    void on_write(
        error_code ec, std::size_t, bool close);

public:
    http_session(
        tcp::socket socket,
        std::shared_ptr<shared_state> const& state);

    void run();
};
}


