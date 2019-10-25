/*
 * flow_server_test.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <fstream>
#include "simple_log.h"
#include "flow_handler.h"
#include "flow_server.h"

class LogFlowHandler : public FlowHandler {

public:
	int do_handle(char *flow_bytes, int size) {
		std::string receive_flow = std::string(flow_bytes, size);
		LOG_DEBUG("start handle the size:%d, flows %s", size, receive_flow.c_str());
		return 0;
	}
};

int main() {
	FlowServer flow_server;
	flow_server.start(3491, new LogFlowHandler());
	return 1;
}
