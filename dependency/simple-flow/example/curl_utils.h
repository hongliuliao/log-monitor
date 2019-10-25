/*
 * curl_utils.h
 *
 *  Created on: Jul 29, 2014
 *      Author: liao
 */

#ifndef CURL_UTILS_H_
#define CURL_UTILS_H_

#include <string>
#include <map>
#include "json/json.h"

class CurlUtils {
public:
	static CURLcode get(std::string &url, std::string &result);

	static CURLcode post(std::string &url, std::map<std::string, std::string> &params, std::string &result);

	static CURLcode post_json(std::string &url, Json::Value &root, std::string &result);
};

#endif /* CURL_UTILS_H_ */
