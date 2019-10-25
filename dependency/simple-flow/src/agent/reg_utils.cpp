/*
 * reg_utils.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */
#include "reg_utils.h"

bool is_match_reg(std::string input, std::string pattern, size_t max_match, std::string *match_str_array, int &matched_size) {
    regex_t reg;
    int com_ret = regcomp(&reg, pattern.c_str(), REG_EXTENDED);
    if(com_ret != 0) {
        LOG_INFO("pattern compile error which pattern:%s, com_ret:%d", pattern.c_str(), com_ret);
        return false;
    }

    regmatch_t pmatch[max_match];
    int status = regexec(&reg, input.c_str(), max_match, pmatch, 0);

    bool ret = false;
    if (status == REG_NOMATCH) {
        LOG_DEBUG("not match for pattern:%s, which input:%s", pattern.c_str(), input.c_str());
    } else if (status == 0) {
        LOG_DEBUG("match success which pattern:%s, input:%s", pattern.c_str(), input.c_str());
        for (size_t i = 0; i < max_match; i++) {
            if (pmatch[i].rm_so == -1 || pmatch[i].rm_eo == -1) {
                continue;
            }
            matched_size++;
            match_str_array[i] = input.substr(pmatch[i].rm_so, (pmatch[i].rm_eo - pmatch[i].rm_so));
            LOG_DEBUG("match success which pattern:%s, input:%s, match_str:%s", pattern.c_str(), input.c_str(), match_str_array[i].c_str());
        }
        ret = true;
    } else {
        char ebuf[80];
        size_t len = regerror(status, &reg, ebuf, 80);
        LOG_INFO("get unkonw status:%d, error_inf:%s, which pattern:%s, input:%s, len:%d", status, ebuf, pattern.c_str(), input.c_str(), len);
    }

    regfree(&reg);
    return ret;
}
