/*
 * log_monitor.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */
#include <iostream>
#include <cstdlib>
#include <map>
#include <time.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "pthread.h"

#include "simple_config.h"
#include "simple_log.h"
#include "file_agent.h"
#include "flow_handler.h"
#include "monitor_handler.h"
#include "http_monitor.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "usage: ./log_monitor [config_file]" << std::endl;
        std::cout << "example: ./log_monitor log_monitor.conf" << std::endl;
        exit(-1);
        return -1;
    }

    char *config_file = argv[1];

    std::map<std::string, std::string> configs;
    get_config_map(config_file, configs);

    LogMonitorHandler &handler = *LogMonitorHandler::get_instance();
    int ret = handler.load_configs(config_file);
    if (ret != 0) {
        LOG_ERROR("load configs error!");
        return -1;
    }

    if (!configs["http_port"].empty()) {
        pthread_t id;
        ret = pthread_create(&id, NULL, start_http_server, &configs);
        if(ret) {
            LOG_ERROR("Create pthread error( for start_http_server)!");
            return 1;
        }
    }

    FileAgent fa;
    fa.set_flow_handler(handler);
    std::string log_file = configs["log_file"];
    char log_file_path[log_file.size() + 1];
    bzero(log_file_path, log_file.size() + 1);
    log_file.copy(log_file_path, log_file.size());

    LOG_INFO("START FILEAGENT for file:%s", log_file_path);
    fa.start(log_file_path);
    return 0;
}
