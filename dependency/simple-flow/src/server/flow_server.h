/*
 * flow_server.h
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */

#ifndef FLOW_SERVER_H_
#define FLOW_SERVER_H_

#include "flow_handler.h"

class FlowServer {

public:

	int start(int port, FlowHandler *handler, int backlog = 10);
};

#endif /* FLOW_SERVER_H_ */
