/*
 * log_monitor.cpp
 *
 *  Created on: 2016-11-28
 *      Author: liao
 */
#include <iostream>
#include <unistd.h>
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

void print_help() {
    std::cout << "usage: ./lm [-s] log_file" << std::endl;
    std::cout << "example: ./lm /tmp/a.log" << std::endl;
}

int main(int argc, char** argv) {
    bool stat_flag = false;
    int interval = 1;

    int opt = -1;
    while ((opt = getopt(argc, argv, "sd:")) != -1) {
        switch (opt) {
        case 's':
            stat_flag = true;
            break;
        case 'd':
            interval = atoi(optarg);
            break;
        default:
            print_help();
            exit(EXIT_FAILURE);
        }
    }
    LMConfig c;
    c._is_stat = stat_flag;
    c._interval = interval;

    if (argc != optind + 1) {
        if ((argv + optind)[0] == NULL) {
            // use stdin for input
            StdInHandler sih(c);
            sih.do_handle();
        }
        print_help();
        return 1;
    }
    std::string log_file = (argv + optind)[0];
    //std::cout << "stat_flag:" << stat_flag << ",pattern:" << pat_str << std::endl;
    //std::cout << "input file:" << log_file << std::endl;

    LMHandler handler(c);

    FileAgent fa;
    fa.set_flow_handler(handler);
    
    char log_file_path[log_file.size() + 1];
    bzero(log_file_path, log_file.size() + 1);
    log_file.copy(log_file_path, log_file.size());

    LOG_INFO("START FILEAGENT for file:%s", log_file_path);
    fa.start(log_file_path);
    return 0;
}
