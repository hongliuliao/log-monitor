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
    FileAgent() {
        _run_time_limit = -1;
    }

    void set_flow_handler(FlowHandler &flow_handler);

    void start(char *input_path, bool is_tail = true);
    
    void start_watch(std::string input_path, bool is_tail = true);

    void set_run_time_limit(int limit) {
        _run_time_limit = limit;
    }
private:
    FlowHandler *_flow_handler;
    int _run_time_limit;

};

#endif /* FILE_AGENT_H_ */
