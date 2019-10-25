.PHONY: all test clean deps tags 

CXX=g++
CXXFLAGS += -g -Wall
LDFLAGS += -pthread

SIMPLE_LOG_LIB_PATH=dependency/simple_log/lib/libsimplelog.a
SIMPLE_FLOW_LIB_PATH=dependency/simple-flow/output/lib/libflowserver.a
deps_dir=dependency
dep_includes=-I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I dependency/simple-flow/output/include
dep_libs=dependency/simple-flow/output/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a
OUTPUT_INC_DIR=output/include
OUTPUT_LIB_DIR=output/lib
OUTPUT_LIB_PATH=$(OUTPUT_LIB_DIR)/liblog_monitor.a
test_dep_includes=-I $(deps_dir)/googletest/googletest/include -I $(OUTPUT_INC_DIR)
test_dep_libs=$(deps_dir)/googletest/googletest/make/gtest_main.a $(OUTPUT_LIB_PATH) $(SIMPLE_LOG_LIB_PATH) $(SIMPLE_FLOW_LIB_PATH)

all: deps prepare lm log_monitor liblog_monitor

deps:
	make -C dependency/googletest/googletest/make
	make -C dependency/simple-flow

prepare:
	mkdir -p output/bin
	mkdir -p $(OUTPUT_INC_DIR)
	mkdir -p $(OUTPUT_LIB_DIR)

log_monitor:
	g++ -g $(dep_includes) src/log_monitor.cpp src/http_monitor.cpp src/monitor_handler.cpp $(dep_libs) -lpthread -o output/bin/log_monitor

package:
	tar -czvf log_monitor.tar.gz output/bin/log_monitor conf resources
	mv log_monitor.tar.gz output/

test: lm log_monitor_test monitor_handler_test

liblog_monitor: src/monitor_handler.cpp
	cp -rf src/*.h $(OUTPUT_INC_DIR)
	ar -rcs output/lib/liblog_monitor.a src/*.o

lm: src/lm.o src/monitor_handler.o
	g++ -g $(dep_includes) src/lm.cpp src/monitor_handler.cpp $(dep_libs) -o output/bin/lm

log_monitor_test: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(dep_includes) $(test_dep_includes) test/log_monitor_test.cpp $(dep_libs) $(test_dep_libs) -o output/bin/log_monitor_test
	
monitor_handler_test:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(dep_includes) $(test_dep_includes) test/monitor_handler_test.cpp $(dep_libs) $(test_dep_libs) -o output/bin/monitor_handler_test

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(dep_includes) $< -o $@

clean:
	rm -rf output/* src/*.o
