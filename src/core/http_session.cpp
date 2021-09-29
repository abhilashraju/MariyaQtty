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

#include "http_session.hpp"
#include "websocket_session.hpp"
#include <iostream>

//------------------------------------------------------------------------------

// Return a reasonable mime type based on the extension of a file.
namespace mariaqtty{
    

    http_session::
    http_session(
        tcp::socket socket,
        std::shared_ptr<shared_state> const& state)
        : socket_(std::move(socket))
        , state_(state)
    {
    }

    void
    http_session::
    run()
    {
        // Read a request
        http::async_read(socket_, buffer_, req_,
            [self = shared_from_this()]
                (error_code ec, std::size_t bytes)
            {
                self->on_read(ec, bytes);
            });
    }

    // Report a failure
    void
    http_session::
    fail(error_code ec, char const* what)
    {
        // Don't report on canceled operations
        if(ec == net::error::operation_aborted)
            return;

        std::cerr << what << ": " << ec.message() << "\n";
    }

    void
    http_session::
    on_read(error_code ec, std::size_t)
    {
        // This means they closed the connection
        if(ec == http::error::end_of_stream)
        {
            socket_.shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        // Handle the error, if any
        if(ec)
            return fail(ec, "read");

        // See if it is a WebSocket Upgrade
        if(websocket::is_upgrade(req_))
        {
            // Create a WebSocket session by transferring the socket
            std::make_shared<websocket_session>(
                std::move(socket_), state_)->run(std::move(req_));
            return;
        }

        // Send the response
        state_->router().handle_http_request(state_->doc_root(), std::move(req_),
            [this](auto&& response)
            {
                // The lifetime of the message has to extend
                // for the duration of the async operation so
                // we use a shared_ptr to manage it.
                using response_type = typename std::decay<decltype(response)>::type;
                auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

    #if 0
                // NOTE This causes an ICE in gcc 7.3
                // Write the response
                http::async_write(this->socket_, *sp,
                    [self = shared_from_this(), sp](
                        error_code ec, std::size_t bytes)
                    {
                        self->on_write(ec, bytes, sp->need_eof()); 
                    });
    #else
                // Write the response
                auto self = shared_from_this();
                http::async_write(this->socket_, *sp,
                    [self, sp](
                        error_code ec, std::size_t bytes)
                    {
                        self->on_write(ec, bytes, sp->need_eof()); 
                    });
    #endif
            });
    }

    void
    http_session::
    on_write(error_code ec, std::size_t, bool close)
    {
        // Handle the error, if any
        if(ec)
            return fail(ec, "write");

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            socket_.shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        // Clear contents of the request message,
        // otherwise the read behavior is undefined.
        req_ = {};

        // Read another request
        http::async_read(socket_, buffer_, req_,
            [self = shared_from_this()]
                (error_code ec, std::size_t bytes)
            {
                self->on_read(ec, bytes);
            });
    }
}

