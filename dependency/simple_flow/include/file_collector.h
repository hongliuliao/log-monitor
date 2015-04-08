/*
 * file_collector.h
 *
 *  Created on: Nov 13, 2014
 *      Author: liao
 */

#ifndef FILE_COLLECTOR_H_
#define FILE_COLLECTOR_H_

#include <string>

class FileCollector {
private:
	bool is_match_file(std::string input, std::string pattern);
public:
	int get_newest_file(std::string input_path, std::string &newest_file_path);
};


#endif /* FILE_COLLECTOR_H_ */
