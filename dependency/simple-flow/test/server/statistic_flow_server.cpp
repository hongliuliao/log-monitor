/*
 * flow_server_test.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <fstream>
#include <sstream>
#include <vector>
#include "simple_log.h"
#include "flow_handler.h"
#include "flow_server.h"
#include "hiredis.h"

class StatisticFlowHandler : public LineFlowHandler {

public:
	std::string get_store_key() {
		struct tm *time_info;
		time_t start;
		time(&start);
		time_info = localtime(&start);
		char day_str[80];
		strftime(day_str, 80, "%Y-%m-%d_%H:%M:%S", time_info);
		return std::string(day_str);
	}

	void incr_in_redis(int delta) {
		redisContext * redis_client = redisConnect("localhost", 6379);
		if(redis_client != NULL && redis_client->err) {
			LOG_ERROR("redis connetct error which err code:%d,msg:%s", redis_client->err, redis_client->errstr);
			return ;
		}

		std::string key = this->get_store_key();
		redisReply* reply = (redisReply*)redisCommand(redis_client, "INCRBY %s %d", key.c_str(), delta);

		freeReplyObject(reply);
		redisFree(redis_client);
		LOG_DEBUG("StatisticFlowHandler incr %s, delta:%d", key.c_str(), delta);
	}

	int handle_lines (std::vector<std::string> lines) {
		incr_in_redis(lines.size());
	}
};

int main() {
	FlowServer flow_server;
	flow_server.start(3492, new StatisticFlowHandler());
	return 1;
}
