/*
 * http_monitor.h
 *
 *  Created on: Apr 7, 2015
 *      Author: liao
 */

#ifndef HTTP_MONITOR_H_
#define HTTP_MONITOR_H_

#include "http_parser.h"

void get_qps_statist_info(Request& request, Response &response);

void get_time_statist_info(Request& request, Response &response);

void static_source_handler(Request& request, Response &res);

void *start_http_server(void *ptr);

#endif /* HTTP_MONITOR_H_ */
