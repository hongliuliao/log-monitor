/*
 * file_agent.h
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */

#ifndef FILE_AGENT_H_
#define FILE_AGENT_H_

#include "flow_handler.h"

class FileAgent {
public:
    void set_flow_handler(FlowHandler &flow_handler);

    void start(char *input_path);
private:
    FlowHandler *_flow_handler;

};

#endif /* FILE_AGENT_H_ */
