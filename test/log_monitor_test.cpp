#include <iostream>
#include <time.h>
#include <reg_utils.h>


int main() {
	struct tm tm;
	strptime("0105 10:39:46", "%m%d %H:%M:%S", &tm);
	tm.tm_year = 2015 - 1900;
	time_t log_time = mktime(&tm);
	std::cout << "log_time:" << log_time << std::endl;
	return 0;
}
