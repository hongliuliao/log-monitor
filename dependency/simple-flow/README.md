simple_flow
===========

simple realtime flow computation 

## 简介
simple_flow 是一个简单的实时流计算框架,通过拉取日志,自己定制flow_handler实现实时计算

## 特点
* linux only
* 继承simple系列的传统,简洁实用
* 安全,可靠 -- 采用拉模式,对数据源系统依赖,影响小

## 依赖
 * [simple_log](https://github.com/hongliuliao/simple_log) 日志组件

## 构建 && 测试
```
  make && make test # 此步骤会生成一个测试的agent
  ./bin/file_agent_test $file_path
```

## 代码示例
```c++
/*
 * file_agent_test.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */
#include <fstream>
#include "simple_log.h"
#include "flow_handler.h"
#include "flow_server.h"
#include "file_agent.h"

class LogFlowHandler : public LineFlowHandler {

public:
    int handle_lines(std::vector<std::string> lines) {
        LOG_DEBUG("handle_lines STARTING ...........................");
        for (unsigned i = 0; i < lines.size(); i++) {
            LOG_DEBUG("start handle line size:%d, line: %s", lines.size(), lines[i].c_str());
        }
        return 0;
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
       fprintf(stderr,"usage %s file_path \n", argv[0]);
       exit(0);
    }
    LogFlowHandler h;
    FileAgent fa;
    fa.set_flow_handler(h);
    fa.start(argv[1]);
    return 0;
}


```
## TODO LIST
 * ~~fileagent实现当文件inode变化时,重新读取新文件~~ DONE at 2014-11-13 check the newest file in dir when no new data
 * ~~epoll方式实现 flow_server~~ DONE at 2014-11-10 
