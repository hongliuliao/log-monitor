/*
 * log_monitor.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */
#include <iostream>
#include <map>
#include <time.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "pthread.h"

#include "simple_config.h"
#include "simple_log.h"
#include "file_agent.h"
#include "flow_handler.h"
#include "reg_utils.h"
#include "json/json.h"
#include "hiredis/hiredis.h"
#include "simple_log.h"
#include "http_server.h"

const char *LINE_REG_NAME = "log_reg";
const char *TIME_FORMAT_NAME = "time_format";
const char *RETAIN_SECONDS_NAME = "retain_seconds";

const int excepted_match_size = 3; // 0: whole, 1: log_time, 2: cost_time
const int index_of_log_time = 1;
const int index_of_cost_time = 2;

class LogMonitorHandler : public LineFlowHandler {

public:
    LogMonitorHandler() {
        line_stat_map = new std::map<time_t, int>();
        cost_time_stat_map = new std::map<time_t, int>();
    }

    ~LogMonitorHandler() {
        if (line_stat_map != NULL) {
            delete line_stat_map;
            line_stat_map = NULL;
        }
        if (cost_time_stat_map != NULL) {
            delete cost_time_stat_map;
            cost_time_stat_map = NULL;
        }
    }

    int load_configs(char *config_file) {
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

    void remove_expired_keys(std::map<time_t,int> &m, time_t expired_time) {
        std::map<time_t,int>::iterator e_index = m.lower_bound(expired_time);
        if (e_index != m.begin()) {
            m.erase(m.begin(), e_index);
            LOG_DEBUG("has expired record is remove!");
        }
    }

    int handle_lines(std::vector<std::string> lines)
    {
        pthread_mutex_lock(&_mutex);

        for (unsigned i = 0; i < lines.size(); i++) {
            int matched_size = 0;
            std::string match_str_array[excepted_match_size];
            bool match_ret = is_match_reg(lines[i], configs[LINE_REG_NAME], excepted_match_size, match_str_array, matched_size);

            if (!match_ret || matched_size < excepted_match_size - 1) {
                LOG_INFO("can not match line for reg:%s, match_ret:%d, matched_size:%d", configs[LINE_REG_NAME].c_str(), match_ret, matched_size);
                continue;
            }

            const char *time_details = match_str_array[index_of_log_time].c_str();
            struct tm tm;
            strptime(time_details, configs[TIME_FORMAT_NAME].c_str(), &tm);
            time_t log_time = mktime(&tm);

            bool has_cost_time = false;
            if (matched_size == excepted_match_size) { // no cost time
                has_cost_time = true;
            }
            LOG_DEBUG("get time_details:%s, log_time:%d, has_cost_time:%d", time_details, log_time, has_cost_time);

            time_t now = time(NULL);
            time_t expired_time = now - atoi(configs[RETAIN_SECONDS_NAME].c_str());

            remove_expired_keys(*line_stat_map, expired_time);
            remove_expired_keys(*cost_time_stat_map, expired_time);

            if (line_stat_map->find(log_time) == line_stat_map->end()) {
                (*line_stat_map)[log_time] = 1;
            } else {
                (*line_stat_map)[log_time]++;
            }

            if (has_cost_time) {
                int cost_time = atoi(match_str_array[index_of_cost_time].c_str());
                if (cost_time_stat_map->find(log_time) == cost_time_stat_map->end()) {
                    (*cost_time_stat_map)[log_time] = cost_time;
                } else {
                    (*cost_time_stat_map)[log_time] += cost_time;
                }
            }
        }

        pthread_mutex_unlock(&_mutex);
        return 0;
    }

    void get_range_map(std::map<time_t,int> & origin_map, std::map<time_t,int> & stat_map, time_t start, time_t end)
    {
        std::map<time_t,int>::iterator start_iter = origin_map.lower_bound(start);
        while(start_iter != origin_map.end()){
            if (start_iter->first >= end) {
                break;
            }
            stat_map[start_iter->first] = start_iter->second;
            start_iter++;
        }
    }

    void get_stat(time_t start, time_t end, std::map<time_t,int> & line_stat, std::map<time_t,int> & cost_stat)
    {
        pthread_mutex_lock(&_mutex);
        get_range_map(*line_stat_map, line_stat, start, end);
        get_range_map(*cost_time_stat_map, cost_stat, start, end);
        pthread_mutex_unlock(&_mutex);
        LOG_INFO("line_stat_map size:%d, cost_time_stat_map size:%d, line_stat size:%d, cost_stat:%d", line_stat_map->size(), cost_time_stat_map->size(), line_stat.size(), cost_stat.size());
    }

private:
    std::map<std::string, std::string> configs;
    std::map<time_t, int> *line_stat_map;
    std::map<time_t, int> *cost_time_stat_map;
    pthread_mutex_t _mutex;
};

LogMonitorHandler handler;

void get_count_statist_info(Request& request, Response &response) {
    std::vector<std::string> times;
    Json::Value root;
    Json::Value json_data;

    time_t now = time(NULL);
    time_t start = now - 10;
    std::map<time_t,int> line_stat, cost_stat;
    handler.get_stat(start, now, line_stat, cost_stat);

    int max_value = 0;
    for (size_t i = 0; i < 5; i++) {
        Json::ArrayIndex index = (Json::ArrayIndex) i;

        time_t stat_time = start + i;
        struct tm *time_info = localtime(&stat_time);
        char day_str[80];
        bzero(day_str, 80);
        strftime(day_str, 80, "%Y-%m-%d_%H:%M:%S", time_info);

        json_data[index]["unit"] = day_str;
        int pv_value = line_stat.find(start + i) == line_stat.end() ? 0 : line_stat[start + i];
        if (pv_value == 0) {
            LOG_WARN("PV VALUE IS ZERO which stat_time:%d", stat_time);
        }
        if (max_value < pv_value) {
            max_value = pv_value;
        }
        json_data[index]["value"] = pv_value;
        LOG_DEBUG("start_time:%s, pv:%d", day_str, pv_value);
    }

    Json::ArrayIndex index = 0;
    root["JSChart"]["datasets"][index]["type"] = "line";
    root["JSChart"]["datasets"][index]["data"] = json_data;

    response.set_body(root);
};

void get_time_statist_info(Request& request, Response &response) {
    std::vector<std::string> times;
    Json::Value root;
    Json::Value json_data;

    time_t now = time(NULL);
    time_t start = now - 5;
    std::map<time_t,int> line_stat, cost_stat;
    handler.get_stat(start, now, line_stat, cost_stat);

    for (size_t i = 0; i < 5; i++) {
        Json::ArrayIndex index = (Json::ArrayIndex) i;

        time_t stat_time = start + i;
        struct tm *time_info = localtime(&stat_time);
        char day_str[80];
        bzero(day_str, 80);
        strftime(day_str, 80, "%Y-%m-%d_%H:%M:%S", time_info);

        json_data[index]["unit"] = day_str;
        int pv_value = line_stat.find(start + i) == line_stat.end() ? 0 : line_stat[start + i];
        int avg_cost_time = 0;
        if (pv_value != 0) {
            avg_cost_time = cost_stat[start + i] / pv_value;
        }
        json_data[index]["value"] = avg_cost_time;
        LOG_DEBUG("start_time:%s, avg_cost_time:%d", day_str, avg_cost_time);
    }

    Json::ArrayIndex index = 0;
    root["JSChart"]["datasets"][index]["type"] = "line";
    root["JSChart"]["datasets"][index]["data"] = json_data;

    response.set_body(root);
};

void static_source_handler(Request& request, Response &res) {
    Json::Value root;
    std::string uri = request.get_request_uri();
    uri.replace(0, 1, "");

    LOG_DEBUG("GET replaced uri:%s", uri.c_str());

    std::string file_path = "resources/";
    file_path += uri;
    std::fstream fs(file_path.c_str());
    std::stringstream ss;
    if(fs) {
        int file_max_size = 500 * 1024;
        char buffer[file_max_size];
        bzero(buffer, file_max_size);
        fs.read(buffer, file_max_size);
        ss << std::string(buffer);
    }
    res.body = ss.str();
    std::string content_type = "text/html";
    if (uri.find(".js") != uri.npos) {
        content_type = "text/javascript;charset=UTF-8";
    }
    res.set_head("Content-Type", content_type);

}

void *start_http_server(void *ptr) {
    // start a http server
    std::map<std::string, std::string> *configs = (std::map<std::string, std::string> *) ptr;
    HttpServer http_server;
    http_server.add_mapping("/get_count", get_count_statist_info);
    http_server.add_mapping("/get_time", get_time_statist_info);
    http_server.add_mapping("/show.html", static_source_handler);
    http_server.add_mapping("/jscharts.js", static_source_handler);
    http_server.start(atoi((*configs)["http_port"].c_str()));
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "usage: ./log_monitor [config_file]" << std::endl;
        std::cout << "example: ./log_monitor log_monitor.conf" << std::endl;
        exit(-1);
        return -1;
    }

    char *config_file = argv[1];

    std::map<std::string, std::string> configs;
    get_config_map(config_file, configs);

    set_log_level(configs["log_level"].c_str());

    int ret = handler.load_configs(config_file);
    if (ret != 0) {
        LOG_ERROR("load configs error!");
        return -1;
    }

    pthread_t id;
    ret = pthread_create(&id, NULL, start_http_server, &configs);
    if(ret) {
        LOG_ERROR("Create pthread error( for start_http_server)!");
        return 1;
    }

    FileAgent fa;
    fa.set_flow_handler(handler);
    std::string log_file = configs["log_file"];
    char log_file_path[log_file.size() + 1];
    bzero(log_file_path, log_file.size() + 1);
    log_file.copy(log_file_path, log_file.size());

    LOG_INFO("START FILEAGENT for file:%s", log_file_path);
    fa.start(log_file_path);
    return 0;
}
