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
#include <memory>
#include <string>
#include <unordered_set>
#include "http_router.hpp"
// Forward declaration
namespace mariaqtty{
class websocket_session;

// Represents the shared server state
class shared_state
{
    public:
    using Router = Router<http::string_body>;
    private:
    std::string doc_root_;

    // This simple method of tracking
    // sessions only works with an implicit
    // strand (i.e. a single-threaded server)
    std::unordered_set<websocket_session*> sessions_;
    Router router_;
public:
    explicit
    shared_state(std::string doc_root);
    auto& router(){return router_;}
    std::string const&
    doc_root() const noexcept
    {
        return doc_root_;
    }

    void join  (websocket_session& session);
    void leave (websocket_session& session);
    void send  (std::string message);
};
}



