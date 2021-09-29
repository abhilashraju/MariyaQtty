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

#include "shared_state.hpp"
#include "websocket_session.hpp"
namespace mariaqtty{
shared_state::
shared_state(std::string doc_root)
    : doc_root_(std::move(doc_root))
{
}

void
shared_state::
join(websocket_session& session)
{
    sessions_.insert(&session);
}

void
shared_state::
leave(websocket_session& session)
{
    sessions_.erase(&session);
}

void
shared_state::
send(std::string message)
{
    auto const ss = std::make_shared<std::string const>(std::move(message));

    for(auto session : sessions_)
        session->send(ss);
}
}

