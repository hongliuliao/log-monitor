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
#include <cctype>
#include "pthread.h"

#include "simple_config.h"
#include "simple_log.h"
#include "file_agent.h"
#include "flow_handler.h"
#include "monitor_handler.h"

void print_help() {
    std::cout << "usage: ./lm [-s] [-d interval] log_file" << std::endl;
    std::cout << "example: ./lm /tmp/a.log" << std::endl;
}

int main(int argc, char** argv) {
    bool stat_flag = false;
    int interval = 1;

    int opt = -1;
    while ((opt = getopt(argc, argv, "shd:")) != -1) {
        switch (opt) {
        case 's':
            stat_flag = true;
            break;
        case 'h':
            print_help();
            exit(EXIT_SUCCESS);
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

    print_help();
    return 0;
}
