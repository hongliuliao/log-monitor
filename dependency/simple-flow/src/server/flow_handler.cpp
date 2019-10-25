/*
 * flow_handler.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: liao
 */
#include <sstream>
#include "flow_handler.h"

int FlowHandler::do_handle(char *flow_bytes, int size) {
	return 0;
}

std::vector<std::string> LineFlowHandler::split_str(std::string &str, char split_char) {
	std::vector<std::string> result;

	std::stringstream ss(str);
	while(ss.good()) {
		std::string temp;
		std::getline(ss, temp, split_char);
		if(!temp.empty()) {
            result.push_back(temp);
		}
	}

	return result;
}

int LineFlowHandler::handle_lines(std::vector<std::string> lines) {
	return 0;
}

int LineFlowHandler::do_handle(char *flow_bytes, int size) {
	std::string flow_str;
	std::string temp_str(flow_bytes, size);
	if(!part_line.empty()) {
        flow_str += part_line;
        part_line.clear();
    }
	flow_str += temp_str;
	std::vector<std::string> lines = split_str(flow_str, '\n');

	if(*(flow_bytes + size - 1) == '\n') {
		handle_lines(lines);
	} else {
		std::vector<std::string> full_lines(lines.begin(), lines.end() - 1);
		handle_lines(full_lines);
		part_line = lines[lines.size() - 1];
	}
	return 0;
};
