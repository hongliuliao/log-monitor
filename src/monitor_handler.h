/*
 * log_monitor_handler.h
 *
 *  Created on: Apr 7, 2015
 *      Author: liao
 */

#ifndef LOG_MONITOR_HANDLER_H_
#define LOG_MONITOR_HANDLER_H_

#include <map>
#include "flow_handler.h"

class LogMonitorHandler : public LineFlowHandler {

public:
    ~LogMonitorHandler() {
        if (line_stat_map != NULL) {
            delete line_stat_map;
            line_stat_map = NULL;
        }
        if (cost_time_stat_map != NULL) {
            delete cost_time_stat_map;
            cost_time_stat_map = NULL;
        }
        if (INSTANCE != NULL) {
            delete INSTANCE;
            INSTANCE = NULL;
        }
    }

    static LogMonitorHandler *get_instance() {
        if (INSTANCE == NULL) {
            INSTANCE = new LogMonitorHandler();
        }
        return INSTANCE;
    }

    int load_configs(char *config_file);

    void remove_expired_keys(std::map<time_t,int> &m, time_t expired_time);

    int handle_lines(std::vector<std::string> lines);

    void get_range_map(std::map<time_t,int> & origin_map, std::map<time_t,int> & stat_map, time_t start, time_t end);

    void get_stat(time_t start, time_t end, std::map<time_t,int> & line_stat, std::map<time_t,int> & cost_stat);

private:
    LogMonitorHandler() {
        line_stat_map = new std::map<time_t, int>();
        cost_time_stat_map = new std::map<time_t, int>();
        last_print_time = 0;
        INSTANCE = NULL;
    }
    std::map<std::string, std::string> configs;
    std::map<time_t, int> *line_stat_map;
    std::map<time_t, int> *cost_time_stat_map;
    static LogMonitorHandler *INSTANCE;
    pthread_mutex_t _mutex;
    time_t last_print_time;
};


#endif /* LOG_MONITOR_HANDLER_H_ */
