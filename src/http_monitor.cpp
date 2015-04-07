/*
 * http_monitor.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: liao
 */
#include <fstream>
#include "http_server.h"
#include "http_monitor.h"
#include "monitor_handler.h"

void get_count_statist_info(Request& request, Response &response) {
    std::vector<std::string> times;
    Json::Value root;
    Json::Value json_data;

    time_t now = time(NULL);
    time_t start = now - 10;
    std::map<time_t,int> line_stat, cost_stat;

    LogMonitorHandler &handler = *LogMonitorHandler::get_instance();
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

    LogMonitorHandler &handler = *LogMonitorHandler::get_instance();
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
