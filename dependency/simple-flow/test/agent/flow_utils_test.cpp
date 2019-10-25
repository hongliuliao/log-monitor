/*
 * flow_utils_test.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: liao
 */
#include "simple_log.h"
#include "flow_utils.h"
#include <curl/curl.h>

void test_get_local_ip() {
    std::string ip = get_local_ip();
    LOG_INFO("GET LOCAL IP:%s", ip.c_str());
}

void test_get_host_name() {
    std::string host = get_host_name();
    LOG_INFO("GET host name:%s", host.c_str());
}

int main() {
    test_get_local_ip();
    test_get_host_name();
    return 0;
}
