/*
 * flow_server_test.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "simple_log.h"
#include "tcp_client.h"
#include "flow_handler.h"
#include "flow_server.h"

class RedisReplFlowHandler : public FlowHandler {

public:
	int total_size;
	TcpClient tcp_client;

	RedisReplFlowHandler() {
		total_size = 0;
		timeval conn_timeout = {2, 0};
		timeval read_timeout = {0, 100};
		int ret = tcp_client.create_socket(conn_timeout, read_timeout);
		if(ret != 0) {
		    LOG_ERROR("create socket error which ret:%d", ret);
		    exit(1);
		    return;
		}
		ret = tcp_client.connect_noblock_socket("localhost", 6380);
		if(ret != 0) {
            LOG_ERROR("connect_noblock_socket error which ret:%d", ret);
            exit(1);
            return;
        }
	}

	int do_handle(char *flow_bytes, int size) {
		total_size += size;

		tcp_client.write_bytes(flow_bytes, size);

		// read all res
		int read_size;
		int buffer_size = 1024;
		char read_buffer[buffer_size];

		while((read_size = tcp_client.read_bytes(read_buffer, buffer_size)) > 0) {
			std::string res(read_buffer, buffer_size);
			LOG_DEBUG("read res size :%d", read_size);
		}

		LOG_INFO("send redis cmds success! total_size:%d", total_size);

		return 0;
	}
};

int main() {
	FlowServer flow_server;
	flow_server.start(3491, new RedisReplFlowHandler());
	return 1;
}
