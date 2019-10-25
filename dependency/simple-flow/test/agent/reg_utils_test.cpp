/*
 * reg_utils_test.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */
#include "reg_utils.h"

int main() {
    size_t pmatch = 3;
    int matched_size = 0;
    std::string match_str_array[pmatch];
    is_match_reg("2014-12-27 10:50:25.069 INFO src/http_parser.h(128): access_log GET /hello status_code:200 cost_time:12 ms, body_size:18", "(.+)\\.[0-9]+ INFO .+ cost_time:([0-9]+) ms", pmatch, match_str_array, matched_size);
    LOG_INFO("MATCHED SIZE:%d", matched_size);
    return 0;
}
