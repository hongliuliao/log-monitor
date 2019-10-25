/*
 * log_monitor_handler.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: liao
 */
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "simple_config.h"
#include "monitor_handler.h"
#include "reg_utils.h"

const char *LINE_REG_NAME = "log_reg";
const char *TIME_FORMAT_NAME = "time_format";
const char *RETAIN_SECONDS_NAME = "retain_seconds";

const int excepted_match_size = 3; // 0: whole, 1: log_time, 2: cost_time
const int index_of_log_time = 1;
const int index_of_cost_time = 2;

StatInfo::StatInfo() {
    t = 0;
    qps = 0;
    has_cost_time = false;
    total_time = 0;
    max_time = 0;
}

int create_stat_log(StatInfo s, std::stringstream &monitor_log) {
    char time_buff[20];
    bzero(time_buff, 20);
    struct tm *timeinfo = localtime(&(s.t));
    strftime(time_buff, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

    monitor_log << "log_time: " << time_buff;
    int qps = s.qps;
    monitor_log << ", QPS:";
    monitor_log << qps;
    if (s.has_cost_time) {
        int ct = s.total_time;
        monitor_log << ", avg_cost_time:" << ct / qps;
        monitor_log << ", max_cost_time:" << s.max_time;
    }
    //LOG_INFO("[log_monitor] %s", monitor_log.str().c_str());
    //std::cout << monitor_log.str() << std::endl;
    return 0;
}

LMConfig::LMConfig() {
    _is_stat = false;
    _interval = 1;
};

LMHandler::LMHandler(LMConfig c) {
    _lmc = c;
    _qps = 0;
    _time = time(NULL);
}

LMHandler::~LMHandler() {}

bool is_num(const std::string &line) {
    for (size_t i = 0; i < line.size(); i++) {
        if (!std::isdigit(line[i])) {
            return false;
        }
    }
    return true;
}

int LMHandler::handle_single(const std::string &line) {
    if (!_lmc._is_stat) {
        std::cout << line << std::endl;
        return 0;
    } 
    time_t ct = time(NULL);
    if (ct - _time < _lmc._interval) {
        _qps++;
        return 0;
    } 
    if (_qps != 0) {
        StatInfo s;
        s.t = _time + _lmc._interval;
        s.qps = _qps / _lmc._interval;
        if (is_num(line)) {
            int cost = atoi(line.c_str());
            s.total_time += cost;
            s.has_cost_time = true;
            if (cost > s.max_time) {
                s.max_time = cost;
            }
        } else {
            s.has_cost_time = false;
        }
        
        std::stringstream ss;
        create_stat_log(s, ss);
        std::cout << ss.str() << std::endl;
    }
    _time = ct;
    _qps = 1;
    return 0;
}

int LMHandler::handle_lines(std::vector<std::string> lines) {
    for (size_t i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        handle_single(line);
    }
    return 0;
}

StdInHandler::StdInHandler(LMConfig c) : LMHandler(c) {}

int StdInHandler::do_handle() {
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        handle_single(line);
    }
    return 0;
}
