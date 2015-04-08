/*
 * flow_handler.h
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */

#ifndef FLOW_HANDLER_H_
#define FLOW_HANDLER_H_

#include <string>
#include <vector>

class FlowHandler {

public:
	virtual int do_handle(char *flow_bytes, int size) ;
};

class LineFlowHandler : public FlowHandler {
private:
	std::string part_line;

	std::vector<std::string> split_str(std::string &str, char split_char);
public:
	int do_handle(char *flow_bytes, int size);

	virtual int handle_lines(std::vector<std::string> lines);
};

#endif /* FLOW_HANDLER_H_ */
