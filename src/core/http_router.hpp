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
#include "beast.hpp"
#include <functional>
#include <map>
namespace mariaqtty{
    inline boost::beast::string_view
    mime_type(boost::beast::string_view path, boost::beast::string_view overrrittern)
    {
        if (overrrittern.size()) return overrrittern;
        using boost::beast::iequals;
        auto const ext = [&path]
        {
            auto const pos = path.rfind(".");
            if(pos == boost::beast::string_view::npos)
                return boost::beast::string_view{};
            return path.substr(pos);
        }();
        if(iequals(ext, ".htm"))  return "text/html";
        if(iequals(ext, ".html")) return "text/html";
        if(iequals(ext, ".php"))  return "text/html";
        if(iequals(ext, ".css"))  return "text/css";
        if(iequals(ext, ".txt"))  return "text/plain";
        if(iequals(ext, ".js"))   return "application/javascript";
        if(iequals(ext, ".json")) return "application/json";
        if(iequals(ext, ".xml"))  return "application/xml";
        if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
        if(iequals(ext, ".flv"))  return "video/x-flv";
        if(iequals(ext, ".png"))  return "image/png";
        if(iequals(ext, ".jpe"))  return "image/jpeg";
        if(iequals(ext, ".jpeg")) return "image/jpeg";
        if(iequals(ext, ".jpg"))  return "image/jpeg";
        if(iequals(ext, ".gif"))  return "image/gif";
        if(iequals(ext, ".bmp"))  return "image/bmp";
        if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
        if(iequals(ext, ".tiff")) return "image/tiff";
        if(iequals(ext, ".tif"))  return "image/tiff";
        if(iequals(ext, ".svg"))  return "image/svg+xml";
        if(iequals(ext, ".svgz")) return "image/svg+xml";
        return "application/text";
    }

    // Append an HTTP rel-path to a local filesystem path.
    // The returned path is normalized for the platform.
    inline std::string
    path_cat(
        boost::beast::string_view base,
        boost::beast::string_view path)
    {
        if(base.empty())
            return path.to_string();
        std::string result = base.to_string();
    #if BOOST_MSVC
        char constexpr path_separator = '\\';
        if(result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());
        for(auto& c : result)
            if(c == '/')
                c = path_separator;
    #else
        char constexpr path_separator = '/';
        if(result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());
    #endif
        return result;
    }
    template<class Body, class Allocator=std::allocator<char>>
    class Router{
    public:
        using request = http::request<Body, http::basic_fields<Allocator>>;
        using string_view= boost::beast::string_view;
        using http_resp = std::tuple<http::status,std::string,std::string>;
        using http_handler=std::function<http_resp(request& req,string_view)>;

        enum  ws_resp_mode {
            None,
            Peer,
            BroadCast
        };
        using ws_resp=std::tuple< ws_resp_mode,std::string>;
        using ws_handler = std::function<ws_resp(string_view)>;
        using ws_filter = std::function<bool(string_view)>;
        using ws_handler_entry=std::tuple<ws_filter,ws_handler>;

    private:
        std::map<std::string,http_handler> http_method_map;
        std::vector<ws_handler_entry> ws_method_map;
        public:
        template<typename Method>
        void register_http_handler(boost::beast::string_view target,Method method){
            http_method_map[std::string{target.data(),target.length()}]=std::move(method);
        }

        template<typename Handler>
        void register_ws_handler(ws_filter filter,Handler handler){
            ws_method_map.emplace_back(std::move(filter),std::move(handler));
        }
        template<typename Handler>
        void register_ws_handler(string_view filterstring, Handler handler){
            auto filter =[=](string_view data){
                return data.starts_with(filterstring);
            };
            register_ws_handler(std::move(filter),std::move(handler));
        }

        template<typename Send>
        void handle_ws_request(string_view data,Send&& send){
           for(auto& v:ws_method_map){
               if(std::get<0>(v)(data)){
                   auto [resp_mode,data]= std::get<1>(v)(data);
                   if(resp_mode!=None){
                       send(resp_mode,data);
                   }
                   return;
               }
           }
        }

        template<typename Send>
        void handle_http_request(boost::beast::string_view doc_root ,request&& req,Send&& send){
             // Returns a bad request response
            auto const bad_request =
            [&req](boost::beast::string_view why)
            {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = why.to_string();
                res.prepare_payload();
                return res;
            };

            // Returns a not found response
            auto const not_found =
            [&req](boost::beast::string_view target)
            {
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + target.to_string() + "' was not found.";
                res.prepare_payload();
                return res;
            };

            // Returns a server error response
            auto const server_error =
            [&req](boost::beast::string_view what)
            {
                http::response<http::string_body> res{http::status::internal_server_error, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "An error occurred: '" + what.to_string() + "'";
                res.prepare_payload();
                return res;
            };
             // Make sure we can handle the method
            if( req.method() != http::verb::get &&
                req.method() != http::verb::head)
                return send(bad_request("Unknown HTTP-method"));

            // Request path must be absolute and not contain "..".
            if( req.target().empty() ||
                req.target()[0] != '/' ||
                req.target().find("..") != boost::beast::string_view::npos)
                return send(bad_request("Illegal request-target"));
                // Build the path to the requested file
            std::string path = path_cat(doc_root, req.target());
            if(req.target().back() == '/')
                path.append("index.html");
            auto& func = http_method_map[std::string{ req.target().data(),req.target().length() }];
           if(func){
                auto [status, content_type,body]=func(req,doc_root);
                if(status == http::status::internal_server_error){
                    return send(server_error("Internal Server Errror"));
                  
                }
                if(status==http::status::not_found){
                    return send(not_found("Target Not Found"));
                }
                auto const size = body.size();
                if(req.method() == http::verb::head)
                {
                    http::response<http::empty_body> res{http::status::ok, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, mime_type(path,""));
                    res.content_length(size);
                    res.keep_alive(req.keep_alive());
                    return send(std::move(res));
                }
                // Respond to GET request
                http::response<http::string_body> res{
                    std::piecewise_construct,
                    std::make_tuple(std::move(body)),
                    std::make_tuple(http::status::ok, req.version())};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, mime_type(path, content_type));
                res.content_length(size);
                res.keep_alive(req.keep_alive());
                return send(std::move(res));
           }
           return send(not_found("Target Not Found"));      
        }
    };
}