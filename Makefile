.PHONY: all test clean deps tags 

CXX=g++
CXXFLAGS += -g -Wall
LDFLAGS += -pthread

deps_dir=dependency
dep_includes=-I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I dependency/simple_flow/include
test_dep_includes=-I $(deps_dir)/googletest/googletest/include
dep_libs=dependency/simple_flow/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a
test_dep_libs=$(deps_dir)/googletest/googletest/make/gtest_main.a

all: deps prepare lm log_monitor

deps:
	make -C dependency/googletest/googletest/make

prepare:
	mkdir -p output/bin

log_monitor:
	g++ -g $(dep_includes) src/log_monitor.cpp src/http_monitor.cpp src/monitor_handler.cpp $(dep_libs) -lpthread -o output/bin/log_monitor

package:
	tar -czvf log_monitor.tar.gz output/bin/log_monitor conf resources
	mv log_monitor.tar.gz output/

test: log_monitor_test

lm: src/lm.o src/monitor_handler.o
	g++ -g $(dep_includes) src/lm.cpp src/monitor_handler.cpp $(dep_libs) -o output/bin/lm

log_monitor_test:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(dep_includes) $(test_dep_includes) test/log_monitor_test.cpp $(dep_libs) $(test_dep_libs) -o output/bin/log_monitor_test

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(dep_includes) $< -o $@

clean:
	rm -rf output/* src/*.o
