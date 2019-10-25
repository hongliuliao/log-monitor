/*
 * reg_utils.h
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */

#ifndef REG_UTILS_H_
#define REG_UTILS_H_

#include <string>
#include <regex.h>
#include "simple_log.h"

bool is_match_reg(std::string input, std::string pattern, size_t nmatch, std::string *match_str_array, int &matched_size);

#endif /* REG_UTILS_H_ */
