/*
 * file_agent_test.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */
#include <fstream>
#include <cstdlib>
#include "simple_log.h"
#include "flow_handler.h"
#include "flow_server.h"
#include "file_agent.h"

class LogFlowHandler : public LineFlowHandler {

public:
    int handle_lines(std::vector<std::string> lines) {
        LOG_DEBUG("handle_lines STARTING ...........................");
        for (unsigned i = 0; i < lines.size(); i++) {
            LOG_DEBUG("start handle line size:%d, line: %s", lines.size(), lines[i].c_str());
        }
        return 0;
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
       fprintf(stderr,"usage %s file_path \n", argv[0]);
       exit(0);
    }
    LogFlowHandler h;
    FileAgent fa;
    fa.set_flow_handler(h);
    fa.start(argv[1]);
    return 0;
}
