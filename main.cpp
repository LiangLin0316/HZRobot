#include "./Include/com/Header/linux_serialport_imp.h"
#include "./Include/http/Header/HttpServer.h"
#include "./Include/task/Header/taskManger.h"
#include "./Include/http/hpp/http_client.hpp"
#include "./Include/robot_config/Header/config.h"
#include "./Include/task/Header/photoTask.h"
#include "./Include/http/Header/HttpBase.h"
#include "./Include/task/Header/taskImp.h"
#include "./Include/mysql/Header/mysql_logs.h"
#include "./Include/common/action.hpp"
#include "./Include/queue/queue.hpp"
#include "./Include/mysql/mysql.hpp"
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/config.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>
#include <vector>


int main(int argc, char *argv[]) {

    Config *config = Config::config_instance();
    //load server config
    config->loadserver_config();
    //load mysql config
    config->loadmysql_config();
    //init mysql;
    mysql_logs *mysq_log = mysql_logs::mysql_instance();
    //write logs
    mysq_log->recordlog(action_begin);
    //start jobs
    mysq_log->recordlog(action_init);
    TaskManger *tmg = TaskManger::get_instance();
    mysq_log->recordlog(action_task);
    tmg->start_del();

    //run the web server
    auto const address = net::ip::make_address(config->ser_config.server_ip);
    auto const port = static_cast<unsigned short>(config->ser_config.server_port);
    auto const doc_root = std::make_shared<std::string>(config->ser_config.doc_root);
    auto const threads = std::max<int>(1, config->ser_config.threads);

    // The io_context is required for all I/O
    net::io_context ioc{threads};

    // Create and launch a listening port
    std::make_shared<listener>(
            ioc,
            tcp::endpoint{address, port},
            doc_root)->run();

    // Run the I/O service on the requested number of threads
    std::vector <std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(
                [&ioc] {
                    ioc.run();
                });
    mysq_log->recordlog(action_web);
    ioc.run();

    return EXIT_SUCCESS;
}
