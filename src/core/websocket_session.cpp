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

#include "websocket_session.hpp"
namespace mariaqtty{
websocket_session::
websocket_session(
    tcp::socket socket,
    std::shared_ptr<shared_state> const& state)
    : ws_(std::move(socket))
    , state_(state)
{
}

websocket_session::
~websocket_session()
{
    // Remove this session from the list of active sessions
    state_->leave(*this);
}

void
websocket_session::
fail(error_code ec, char const* what)
{
    // Don't report these
    if( ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

void
websocket_session::
on_accept(error_code ec)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "accept");

    // Add this session to the list of active sessions
    state_->join(*this);

    // Read a message
    ws_.async_read(
        buffer_,
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_read(ec, bytes);
        });
}

void
websocket_session::
on_read(error_code ec, std::size_t)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "read");

    // Send to all connections
  
    auto data = beast::buffers_to_string(buffer_.data());
    // Clear the buffer
    buffer_.consume(buffer_.size());

    state_->router().handle_ws_request(data,[&](Router::ws_resp_mode m,auto& data){   
        switch (m)
        {
        case Router::None:
            break;
        case Router::Peer:
            send(std::make_shared<std::string>(data));
            break;
        case Router::BroadCast:
            state_->send(data);
            break;
        default:
            break;
        }
        
    });
    
   

    // Read another message
    ws_.async_read(
        buffer_,
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_read(ec, bytes);
        });
}

void
websocket_session::
send(std::shared_ptr<std::string const> const& ss)
{
    // Always add to queue
    queue_.push_back(ss);

    // Are we already writing?
    if(queue_.size() > 1)
        return;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(*queue_.front()),
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_write(ec, bytes);
        });
}

void
websocket_session::
on_write(error_code ec, std::size_t)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "write");

    // Remove the string from the queue
    queue_.erase(queue_.begin());

    // Send the next message if any
    if(! queue_.empty())
        ws_.async_write(
            net::buffer(*queue_.front()),
            [sp = shared_from_this()](
                error_code ec, std::size_t bytes)
            {
                sp->on_write(ec, bytes);
            });
}
}

