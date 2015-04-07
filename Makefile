dep_includes=-I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I dependency/simple_flow/include
dep_libs=dependency/simple_flow/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a

all: 
	mkdir -p bin
	g++ -g $(dep_includes) src/log_monitor.cpp src/http_monitor.cpp src/monitor_handler.cpp $(dep_libs) -lpthread -lcurl -o bin/log_monitor
	tar -czvf log_monitor.tar.gz bin/log_monitor conf resources

test: log_monitor_test

log_monitor_test:
	g++ -g $(dep_includes) test/log_monitor_test.cpp $(dep_libs) -o bin/log_monitor_test

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@