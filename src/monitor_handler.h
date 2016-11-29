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

struct StatInfo {
    int qps;
    int total_time;
    int max_time;
};

class LogMonitorHandler : public LineFlowHandler {

public:
    ~LogMonitorHandler() {
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

    void remove_expired_keys(std::map<time_t, StatInfo> &m, time_t expired_time);

    int handle_lines(std::vector<std::string> lines);

    void get_range_map(std::map<time_t, StatInfo> & origin_map, std::map<time_t, StatInfo> & stat_map, time_t start, time_t end);

    void get_stat(time_t start, time_t end, std::map<time_t, StatInfo> & line_stat);

private:
    LogMonitorHandler() {
        last_print_time = 0;
        INSTANCE = NULL;
    }
    std::map<std::string, std::string> configs;
    std::map<time_t, StatInfo> stat_map;
    static LogMonitorHandler *INSTANCE;
    pthread_mutex_t _mutex;
    time_t last_print_time;
};

class LMConfig {
    public:
        LMConfig();
        std::string _pat;
        bool _is_stat;
        int _interval;
};

class LMHandler : public LineFlowHandler {

public:
    LMHandler(LMConfig c);

    virtual ~LMHandler();

    int handle_lines(std::vector<std::string> lines);

    int handle_single(std::string line);

private:
    LMConfig _lmc;
    int _qps;
    time_t _time;
};

class StdInHandler : public LMHandler {
    public:
        StdInHandler(LMConfig c);
        int do_handle();
        int handle_lines(std::vector<std::string> lines) {/*no use*/};
    private:
        LMConfig _lmc;
};

#endif /* LOG_MONITOR_HANDLER_H_ */
