/*
 * log_index.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include <time.h>
#include "file_agent.h"
#include "curl/curl.h"
#include "curl_utils.h"
#include "flow_handler.h"

class LogIndexHandler : public LineFlowHandler {

public:
    int handle_lines(std::vector<std::string> lines) {

        for (unsigned i = 0; i < lines.size(); i++) {
            std::string &line = lines[i];

            Json::Value params;
            params["log_content"] = line;
            time_t now = time(NULL);
            params["create_time"] = (Json::Int64) now;

            int format_time_len = 17;
            char format_time_str[format_time_len];
            bzero(format_time_str, format_time_len);
            struct tm *time_info = localtime(&now);
            strftime(format_time_str, format_time_len, "%Y-%m-%d", time_info);

            std::string result;
            std::stringstream ss;
            ss << url << format_time_str;
            std::string real_url = ss.str();
            CURLcode code = CurlUtils::post_json(real_url, params, result);
            std::cout << "get code:" << code << ",result:" << result << std::endl;
        }
        return 0;
    }

    std::string url;
};

int main(int argc, char** argv) {
    if(argc < 3) {
        std::cout << "usage: ./log_index [log_file] [es_url_prefix]" << std::endl;
        std::cout << "example: ./log_index /home/liao/programs/nginx/logs/access.log http://localhost:9200/mytest/accesslog-" << std::endl;
        exit(-1);
        return -1;
    }
    LogIndexHandler handler;
    handler.url = std::string(argv[2]);

    FileAgent fa;
    fa.set_flow_handler(handler);
    fa.start(argv[1]);
    return 0;
}
