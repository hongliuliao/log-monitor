/*
 * file_agent.cpp, read file and send to flow_server
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <fstream>

#include "simple_log.h"

#include "tcp_client.h"
#include "file_reader.h"
#include "file_agent.h"

void FileAgent::set_flow_handler(FlowHandler &flow_handler) {
    _flow_handler = &flow_handler;
};

void FileAgent::start(char *input_path, bool is_tail) {
    this->start_watch(input_path, is_tail);
}

void FileAgent::start_watch(std::string input_path, bool is_tail) {
    FileReader file_reader(input_path, is_tail);
    int req_size = 4096;
    char req_buffer[req_size];
    int read_size = 0;

    int retry_status = 0;
    int run_time;
    while(1) {
        if(!retry_status) {
            bzero(req_buffer,req_size);
            // TODO change check mode , check (the file change && offset == filesize)
            if(file_reader.read(req_buffer, req_size, read_size) != 0) {
                sleep(1);
                if(file_reader.read(req_buffer, req_size, read_size) != 0) {
                    // check if has new file
                    file_reader.check_and_reset();
                    continue;
                }
            }
        }

        int n = _flow_handler->do_handle(req_buffer, read_size);
        if (n < 0) {
             LOG_ERROR("ERROR writing to socket");
             sleep(1);
             retry_status = 1;
             continue;
        }

        retry_status = 0;
        if (_run_time_limit > 0 && ++run_time >= _run_time_limit) {
            break;
        }
    }
}
