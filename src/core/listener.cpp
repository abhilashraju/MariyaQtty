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
#include "http_session.hpp"
#include <iostream>
namespace mariaqtty{
    listener::
    listener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        std::shared_ptr<shared_state> const& state)
        : acceptor_(ioc)
        , socket_(ioc)
        , state_(state)
    {
        error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true));
        if(ec)
        {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    void
    listener::
    run()
    {
        // Start accepting a connection
        acceptor_.async_accept(
            socket_,
            [self = shared_from_this()](error_code ec)
            {
                self->on_accept(ec);
            });
    }

    // Report a failure
    void
    listener::
    fail(error_code ec, char const* what)
    {
        // Don't report on canceled operations
        if(ec == net::error::operation_aborted)
            return;
        std::cerr << what << ": " << ec.message() << "\n";
    }

    // Handle a connection
    void
    listener::
    on_accept(error_code ec)
    {
        if(ec)
            return fail(ec, "accept");
        else
            // Launch a new session for this connection
            std::make_shared<http_session>(
                std::move(socket_),
                state_)->run();

        // Accept another connection
        acceptor_.async_accept(
            socket_,
            [self = shared_from_this()](error_code ec)
            {
                self->on_accept(ec);
            });
    }

}
