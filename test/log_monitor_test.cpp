#include <iostream>
#include <time.h>
#include <reg_utils.h>
#include "gtest/gtest.h"

TEST(CommonTest, test_log_time) {
	struct tm tm;
	strptime("0105 10:39:46", "%m%d %H:%M:%S", &tm);
	tm.tm_year = 2015 - 1900;
	time_t log_time = mktime(&tm);
	std::cout << "log_time:" << log_time << std::endl;
}
