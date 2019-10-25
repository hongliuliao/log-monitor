/*
 * curl_utils.cpp
 *
 *  Created on: Jul 29, 2014
 *      Author: liao
 */
#include <string>
#include <sstream>
#include "curl/curl.h"
#include "curl_utils.h"
#include "json/json.h"

#include "simple_log.h"

int res_writer(char *data, size_t size, size_t nmemb, std::string *write_data) {
	if(write_data == NULL) {
		return 0;
	}
	write_data->append(data, size * nmemb);
	return size * nmemb;
}

CURLcode CurlUtils::get(std::string &url, std::string &result) {
	CURL *curl = curl_easy_init();
	if(!curl) {
		LOG_WARN("curl_easy_init error!");
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	CURLcode res_code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, res_writer);

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

	res_code = curl_easy_perform(curl);

	if(res_code != CURLE_OK) {
		LOG_ERROR("curl_easy_perform faild : %s, url:%s", curl_easy_strerror(res_code), url.c_str());
	}
	curl_easy_cleanup(curl);
	return res_code;
}

CURLcode CurlUtils::post(std::string &url, std::map<std::string, std::string> &params, std::string &result) {
    CURL *curl = curl_easy_init();
    if(!curl) {
        LOG_WARN("curl_easy_init error!");
        return CURLE_FAILED_INIT;
    }

    int i = 0;
    std::stringstream post_url;
    for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); ++it) {
        if(i++ != 0) {
            post_url << "&";
        }
        post_url << it->first << "=" << it->second;
    }

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_url.str().c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    CURLcode res_code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, res_writer);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

    res_code = curl_easy_perform(curl);

    if(res_code != CURLE_OK) {
        LOG_ERROR("curl_easy_perform faild : %s, url:%s", curl_easy_strerror(res_code), url.c_str());
    }

    curl_easy_cleanup(curl);
    return res_code;
}

CURLcode CurlUtils::post_json(std::string &url, Json::Value &root, std::string &result) {
    CURL *curl = curl_easy_init();
    if(!curl) {
        LOG_WARN("curl_easy_init error!");
        return CURLE_FAILED_INIT;
    }

    Json::FastWriter writer;
    std::string json = writer.write(root);

    LOG_DEBUG("get post json:%s", json.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    CURLcode res_code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, res_writer);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

    res_code = curl_easy_perform(curl);

    if(res_code != CURLE_OK) {
        LOG_ERROR("curl_easy_perform faild : %s, url:%s", curl_easy_strerror(res_code), url.c_str());
    }

    curl_easy_cleanup(curl);
    return res_code;
}
