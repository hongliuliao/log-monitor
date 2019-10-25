/*
 * statistic_server.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "json/json.h"
#include "hiredis.h"
#include "simple_log.h"
#include "http_server.h"

std::vector<std::string> get_cmds(std::vector<std::string> &times) {
	std::vector<std::string> result;
	struct tm *time_info;
	time_t start;
	time(&start);

	// get recent 5s data
	for (unsigned i = 5; i > 0; i--) {
		time_t stat_time = start - i;
		time_info = localtime(&stat_time);
		char day_str[80];
		bzero(day_str, 80);
		strftime(day_str, 80, "%Y-%m-%d_%H:%M:%S", time_info);
		std::string cmd("get ");
		cmd += day_str;
		LOG_DEBUG("build redis cmd:%s", cmd.c_str());
		result.push_back(cmd);
		times.push_back(std::string(day_str));
	}
	return result;
}

int send_commands(std::vector<std::string> cmds, std::vector<std::string> times, Json::Value &json_data) {
	redisContext *rc = redisConnect("localhost", 6379);

	for(size_t i=0;i<cmds.size();i++) {
		redisAppendCommand(rc, cmds[i].c_str());
	}

	for(size_t i=0;i<cmds.size();i++) {
		redisReply *reply;
		redisGetReply(rc, (void **)&reply);

		if(reply->type == REDIS_REPLY_ERROR) {
			LOG_WARN("redis return error which msg:%s", reply->str);
			continue;
		}
		int pv_value = reply->type == REDIS_REPLY_NIL ? 0 : atoi(reply->str);

		Json::ArrayIndex index = (Json::ArrayIndex) i;
		json_data[index]["unit"] = times[i];
		json_data[index]["value"] = pv_value;
		LOG_DEBUG("start_time:%s, pv:%d", times[i].c_str(), pv_value);

		if(reply != NULL) {
			freeReplyObject(reply);
		}
	}

	redisFree(rc);
	return 0;
}

void get_statist_info(Request& request, Response &response) {
	std::vector<std::string> times;
	Json::Value root;
	Json::Value json_data;

	std::vector<std::string> cms = get_cmds(times);
	LOG_DEBUG("get cmds size:%d", cms.size());
	send_commands(cms, times, json_data);

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

	std::string file_path = "test/";
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
	res._body = ss.str();
	std::string content_type = "text/html";
	res.set_head("Content-Type", content_type);
}

int main() {
	HttpServer http_server;
	http_server.add_mapping("/get_data", get_statist_info);
	http_server.add_mapping("/show.html", static_source_handler);
	http_server.start(3493);
}
