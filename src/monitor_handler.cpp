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
#include "simple_log.h"
#include "monitor_handler.h"
#include "reg_utils.h"

const char *LINE_REG_NAME = "log_reg";
const char *TIME_FORMAT_NAME = "time_format";
const char *RETAIN_SECONDS_NAME = "retain_seconds";

const int excepted_match_size = 3; // 0: whole, 1: log_time, 2: cost_time
const int index_of_log_time = 1;
const int index_of_cost_time = 2;

LogMonitorHandler *LogMonitorHandler::INSTANCE = NULL;

int LogMonitorHandler::load_configs(char *config_file) {
    get_config_map(config_file, configs);
    if (configs[LINE_REG_NAME].empty()) {
        LOG_ERROR("log_reg is empty!");
        return -1;
    }
    if (configs[TIME_FORMAT_NAME].empty()) {
        LOG_ERROR("time_format is empty!");
        return -1;
    }
    return 0;
}

void LogMonitorHandler::remove_expired_keys(std::map<time_t, StatInfo> &m, time_t expired_time) {
    std::map<time_t, StatInfo>::iterator e_index = m.lower_bound(expired_time);
    if (e_index != m.begin()) {
        m.erase(m.begin(), e_index);
        LOG_DEBUG("has expired record is remove!");
    }
}

int LogMonitorHandler::handle_lines(std::vector<std::string> lines) {
    pthread_mutex_lock(&_mutex);

    for (unsigned i = 0; i < lines.size(); i++) {
        int matched_size = 0;
        std::string match_str_array[excepted_match_size];
        bool match_ret = is_match_reg(lines[i], configs[LINE_REG_NAME], excepted_match_size, match_str_array, matched_size);

        if (!match_ret || matched_size < excepted_match_size - 1) {
            LOG_DEBUG("can not match line for reg:%s, match_ret:%d, matched_size:%d", configs[LINE_REG_NAME].c_str(), match_ret, matched_size);
            continue;
        }

        const char *time_details = match_str_array[index_of_log_time].c_str();
        struct tm tm;
        if (configs["set_year"] == "true") {
            time_t now = time(NULL);
            tm = *localtime(&now);
        }
        strptime(time_details, configs[TIME_FORMAT_NAME].c_str(), &tm);
        time_t log_time = mktime(&tm);

        bool has_cost_time = false;
        if (matched_size == excepted_match_size) { // no cost time
            has_cost_time = true;
        }
        LOG_DEBUG("get time_details:%s, log_time:%d, has_cost_time:%d", time_details, log_time, has_cost_time);

       // time_t now = time(NULL);
        time_t expired_time = log_time - atoi(configs[RETAIN_SECONDS_NAME].c_str());

        remove_expired_keys(stat_map, expired_time);

        if (stat_map.find(log_time) == stat_map.end()) {
            StatInfo si = {0, 0, 0};
            stat_map[log_time] = si;
        }

        stat_map[log_time].qps = stat_map[log_time].qps + 1;
        if (has_cost_time) {
            int cost_time = atoi(match_str_array[index_of_cost_time].c_str());
            stat_map[log_time].total_time += cost_time;
            if (cost_time > stat_map[log_time].max_time) {
                stat_map[log_time].max_time = cost_time;
            }
        }

        if (last_print_time == 0) {
            last_print_time = log_time;
        } else if (last_print_time != log_time) {
            std::stringstream monitor_log;
            char time_buff[20];
            bzero(time_buff, 20);
            struct tm *timeinfo = localtime(&last_print_time);
            strftime(time_buff, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

            monitor_log << "log_time: " << time_buff;
            int qps = stat_map[last_print_time].qps;
            monitor_log << ", QPS:";
            monitor_log << qps;
            if (has_cost_time) {
                int ct = stat_map[last_print_time].total_time;
                monitor_log << ", avg_cost_time:" << ct / qps;
                monitor_log << ", max_cost_time:" << stat_map[last_print_time].max_time;
            }
            last_print_time = log_time;
            LOG_INFO("[log_monitor] %s", monitor_log.str().c_str());
        }
    }

    pthread_mutex_unlock(&_mutex);
    return 0;
}

void LogMonitorHandler::get_range_map(std::map<time_t, StatInfo> & origin_map, std::map<time_t, StatInfo> & stat_map, time_t start, time_t end) {
    std::map<time_t, StatInfo>::iterator start_iter = origin_map.lower_bound(start);
    while(start_iter != origin_map.end()){
        if (start_iter->first >= end) {
            break;
        }
        stat_map[start_iter->first] = start_iter->second;
        start_iter++;
    }
}

void LogMonitorHandler::get_stat(time_t start, time_t end, std::map<time_t, StatInfo> & line_stat) {
    pthread_mutex_lock(&_mutex);
    get_range_map(stat_map, line_stat, start, end);
    pthread_mutex_unlock(&_mutex);
    LOG_INFO("line_stat_map size:%u, line_stat size:%u", stat_map.size(), line_stat.size());
}

LMConfig::LMConfig() {
    _is_stat = false;
};

LMHandler::LMHandler(LMConfig c) {
    _lmc = c;
    _qps = 0;
    _time = time(NULL);
}

LMHandler::~LMHandler() {}

int LMHandler::handle_single(std::string line) {
    if (!_lmc._is_stat) {
        std::cout << line << std::endl;
        return 0;
    } 
    time_t ct = time(NULL);
    if (ct == _time) {
        _qps++;
        return 0;
    } 
    if (_qps != 0) {
        std::cout << _qps << std::endl;
    }
    _time = ct;
    _qps = 1;
    return 0;
}

int LMHandler::handle_lines(std::vector<std::string> lines) {
    for (int i = 0; i < lines.size(); i++) {
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
