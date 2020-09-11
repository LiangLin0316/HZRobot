//
// Created by Lin on 2020/8/21.
//
//#include "../Header/HttpBase.h"
#include "../Header/HttpServer.h"
#include "../../shine_serial/shine_serial.hpp"
#include "../hpp/http_client.hpp"
#include "../../util/string.hpp"
#include <iostream>
#include <sstream>
#include "../../task/Header/taskManger.h"
#include "../../task/Header/photoTask.h"
#include <fstream>
#include "../../xml/Header/tinyxml.h"
#define MAX_CHAR_SIZE 102400


// transfor beast to shine
void transstring(beast::string_view &beast_str, shine::string &shine_str) {
    stringstream sstr;
    sstr.clear();
    sstr << beast_str;
    sstr >> shine_str;
    std::cout << "Here:" << shine_str << std::endl;
}

// transfor beast to str
void transstring2(beast::string_view &beast_str, std::string &str) {
    stringstream sstr;
    sstr.clear();
    sstr << beast_str;
    sstr >> str;
    std::cout << "Here2:" << str << std::endl;
}

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path) {
    using beast::iequals;
    auto const ext = [&path] {
        auto const pos = path.rfind(".");
        if (pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm")) return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php")) return "text/html";
    if (iequals(ext, ".css")) return "text/css";
    if (iequals(ext, ".txt")) return "text/plain";
    if (iequals(ext, ".js")) return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml")) return "application/xml";
    if (iequals(ext, ".swf")) return "application/x-shockwave-flash";
    if (iequals(ext, ".flv")) return "video/x-flv";
    if (iequals(ext, ".png")) return "image/png";
    if (iequals(ext, ".jpe")) return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg")) return "image/jpeg";
    if (iequals(ext, ".gif")) return "image/gif";
    if (iequals(ext, ".bmp")) return "image/bmp";
    if (iequals(ext, ".ico")) return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif")) return "image/tiff";
    if (iequals(ext, ".svg")) return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
        beast::string_view base,
        beast::string_view path) {
    if (base.empty())
        return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
        class Body, class Allocator,
        class Send>
void
handle_request(
        beast::string_view doc_root,
        http::request <Body, http::basic_fields<Allocator>> &&req,
        Send &&send) {
    // Returns a bad request response
    auto const bad_request =
            [&req](beast::string_view why) {
                http::response <http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = std::string(why);
                res.prepare_payload();
                return res;
            };

    // Returns a not found response
    auto const not_found =
            [&req](beast::string_view target) {
                http::response <http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + std::string(target) + "' was not found.";
                res.prepare_payload();
                return res;
            };

    // Returns a server error response
    auto const server_error =
            [&req](beast::string_view what) {
                http::response <http::string_body> res{http::status::internal_server_error, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "An error occurred: '" + std::string(what) + "'";
                res.prepare_payload();
                return res;
            };

    // Just do for josn
    auto const comm_response =
            [&req](shine::string what) {
                http::response <http::string_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "application/json");
                res.keep_alive(req.keep_alive());
                if (what.length()) {
                    shine::string json_str = what;
                    shine::json json;
                    json.decode(json_str);
                    res.body() = json.encode();
                }
                res.prepare_payload();
                return res;
            };


    // Make sure we can handle the method
    if (req.method() != http::verb::post &&
        req.method() != http::verb::get &&
        req.method() != http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if (req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos ||
        req.target().find("//") != beast::string_view::npos)
        return send(bad_request("Illegal request-target"));


    std::string path = path_cat(doc_root, req.target());

    //del transform data
    if (path.find("./api/v2/") != path.npos) {
        Config *config = Config::config_instance();
        std::cout << "transform to robot" << std::endl;
        shine::http::sync_client sync_c;
        sync_c.set_recv_timeout(config->ser_config.timeout);
        sync_c.get_request().set_host(config->ser_config.robot_ip);

        beast::string_view beast_type = req[http::field::content_type];
        if (!beast_type.empty()) {
            shine::string shine_str;
            shine_str.clear();
            transstring(beast_type, shine_str);
            sync_c.get_request().set_content_type(shine_str);
        }
        switch (req.method()) {
            case http::verb::post: {
                std::cout << "http::verb::post:" << std::endl;
                sync_c.get_request().set_method(http_method_post);
                break;
            }
            case http::verb::get: {
                std::cout << "http::verb::get:" << shine::string(req.body()) << std::endl;
                sync_c.get_request().set_method(http_method_get);
                break;
            }
            case http::verb::delete_: {
                std::cout << "http::verb::delete_:" << shine::string(req.body()) << std::endl;
                sync_c.get_request().set_method(http_method_delete_);
                break;
            }
            case http::verb::put: {
                std::cout << "http::verb::put:" << shine::string(req.body()) << std::endl;
                sync_c.get_request().set_method(http_method_put);
                break;
            }
            default:
                break;
        }
        sync_c.get_request().set_body(shine::string(req.body()));
        std::string str_target = path_cat("", req.target());
        sync_c.get_request().set_url(str_target);
        if (sync_c.call()) {
            shine::string tmp;
            tmp = sync_c.get_response().get_body();
            std::cout << tmp << std::endl;
            return send(comm_response(tmp));

        } else {
            return send(bad_request("{  \"msg\":\"transmit faild!\"}"));
        }
    }

        //del Robot post data
    else if (path.find("./robot/tips") != path.npos) {
        beast::string_view content_type = req[http::field::content_type];
        if (content_type != "application/json") {
            return send(bad_request("error request"));};
        //del with thread
        TaskManger *tmg = TaskManger::get_instance();
        mysql_logs *mysq_log = mysql_logs::mysql_instance();
        Config *config = Config::config_instance();
        shine::json json;
        shine::string tmp;
        tmp = req.body();
        json.decode(tmp);
        try {
            std::cout<<std::endl;
            int mode = atoi(json.get_root().find_kv_child("ACTION")->get_value());
            //write logs
            std::string marks ="接收到任务类型：";
            marks+=std::to_string(mode);
            mysq_log->recordlog(action_post,marks);
            switch (mode)
            {
                case run_photo:
                {
                    config->loadphoto_config();
                    tmg->start_product(new PhotoTask(new Detector()));
                    break;
                }
                case run_pick_sound:
                {
                    tmg->start_product(new PhotoTask(new Detector()));
                    break;
                }
                case run_pick_temperature:
                {
                    tmg->start_product(new PhotoTask(new Detector()));
                    break;
                }
                default:
                    break;
            }
        }
        catch(...)
        {

        }

        return send(comm_response(""));
    }
    else if (path.find("./robot/configs") != path.npos) {
        char buf[10];
        long ncount = 0;
        int n_npos = int(std::string::npos);
        stringstream strstream;
        strstream << req.body();
        char reading_line[MAX_CHAR_SIZE];
        bool b_delimit = false;
        std::string s_delimit;
        std::string s_postfix;
        fstream f_config;
        beast::string_view content_type = req[http::field::content_type];
        //just for dest api
        if(path.find("/mysql_config") == path.npos && path.find("/photo_config")== path.npos && path.find("/server_config")== path.npos)
        {
            return send(bad_request("error request"));
        }

        if ((n_npos = content_type.find("boundary=")) == beast::string_view::npos) {
            return send(bad_request("error request"));
        } else {
            b_delimit = true;
            beast::string_view beast_delimit;
            beast_delimit = content_type.substr(n_npos + strlen("boundary="));
            transstring2(beast_delimit, s_delimit);
        }
        while (strstream.getline(reading_line, MAX_CHAR_SIZE)) {
            std::string s_line = reading_line;
            ncount += (strlen(reading_line) + 1);
            memset(reading_line, 0, sizeof(reading_line));
            if (b_delimit && s_line.find("Content-Disposition:") != string::npos) {
                string config_name;
                s_postfix = s_line.substr(s_line.find("."), s_line.rfind("\"") - s_line.find("."));
                if(s_postfix!=".xml")
                {

                    return send(bad_request("error request"));
                }

                else
                {
                    if(s_line.find("mysql_config")!=std::string::npos)
                        config_name = "mysql_config" + s_postfix;
                    else if(s_line.find("photo_config")!=std::string::npos)
                        config_name = "photo_config" + s_postfix;
                    else if(s_line.find("server_config")!=std::string::npos)
                        config_name = "server_config" + s_postfix;
                    else
                    {
                        return send(bad_request("error request"));
                    }
                }
                f_config.open(config_name, ios::binary | ios::ate | ios::out);
                continue;
            }

            if (b_delimit && s_line.find("Content-Type:") != string::npos) {
                char devnull[MAX_CHAR_SIZE];
                memset(devnull, 0, sizeof(devnull));

                strstream.getline(devnull, MAX_CHAR_SIZE);
                ncount += (strlen(devnull) + 1);
                //+2 next line   -2 before line
                while (strstream && ncount +2 < req.body().rfind(s_delimit) - 2) {
                    memset(buf,0, sizeof(buf));
                    strstream.read(buf, 1);
                    std::cout << buf << std::endl;
                    f_config.write(buf, strstream.gcount());
                    ncount += strstream.gcount();
                }
                break;
            }
        }
        if (f_config.is_open())
            f_config.close();

        Config *config = Config::config_instance();
        config->loadphoto_config();
        config->loadserver_config();
        config->loadmysql_config();

        return send(comm_response(""));
    }

    //handle post requests
    if (req.method() == http::verb::post) {
        // We're done with the response so delete it
        beast::string_view content_type = req[http::field::content_type];
        std::cout << content_type << std::endl;
        shine::string json_str = shine::string(req.body());
        std::cout << json_str << std::endl;
        if (content_type == "application/json") {
            return send(bad_request(req.target()));
        } else {
        }
        fstream out;
        out.open("./data.txt", ios::ate | ios::out | ios::binary);
        out << req.body();
        out.close();
        return send(bad_request("POST"));
    }

    //handle get requests
    if (req.method() == http::verb::get) {
        // We're done with the response so delete it
        return send(bad_request("Get"));
    }

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if (ec)
        return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == http::verb::head) {
        http::response <http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    // Respond to GET request
    http::response <http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}

//------------------------------------------------------------------------------

// Report a failure
void
fail(beast::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

//------------------------------------------------------------------------------

void session::run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    net::dispatch(stream_.get_executor(),
                  beast::bind_front_handler(
                          &session::do_read,
                          shared_from_this()));
}

void session::do_read() {
    // Make the request empty before reading,
    // otherwise the operation behavior is undefined.
    req_ = {};

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request
    http::async_read(stream_, buffer_, req_,
                     beast::bind_front_handler(
                             &session::on_read,
                             shared_from_this()));
}

void session::on_read(
        beast::error_code ec,
        std::size_t bytes_transferred) {


    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
        return do_close();

    if (ec)
        return fail(ec, "read");

    // Send the response
    handle_request(*doc_root_, std::move(req_), lambda_);
}

void session::on_write(
        bool close,
        beast::error_code ec,
        std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    if (close) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
    }

    // We're done with the response so delete it
    res_ = nullptr;

    // Read another request
    do_read();
}

void session::do_close() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

listener::listener(net::io_context &ioc, tcp::endpoint endpoint, std::shared_ptr<std::string const> const &doc_root)
        : ioc_(ioc), acceptor_(net::make_strand(ioc)), doc_root_(doc_root) {
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }
}


void listener::run() {
    do_accept();
}

void listener::do_accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                    &listener::on_accept,
                    shared_from_this()));
}

void listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        fail(ec, "accept");
    } else {
        // Create the session and run it
        std::make_shared<session>(
                std::move(socket),
                doc_root_)->run();
    }

    // Accept another connection
    do_accept();
}


