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

class StatInfo {
    public:
        StatInfo();
        time_t t;
        int qps;
        bool has_cost_time;
        int total_time;
        int max_time;
};

int create_stat_log(StatInfo s, std::stringstream &ss);

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

    int handle_single(const std::string &line);

private:
    LMConfig _lmc;
    int _qps;
    time_t _time;
};

class StdInHandler : public LMHandler {
    public:
        StdInHandler(LMConfig c);
        int do_handle();
        int handle_lines(std::vector<std::string> lines) { return 0; /*no use*/ };
    private:
        LMConfig _lmc;
};

#endif /* LOG_MONITOR_HANDLER_H_ */
