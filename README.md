log_monitor
===========


## 功能介绍	
对qps以及接口的平均耗时进行统计
## 特点	
(1)轻量: 共219KB(代码量约300行)	
(2)低侵入: 基于日志文件进行统计,无需修改原程序
## 使用说明
(1) 二进制包:下载工具包 log_monitor.tar.gz,并解压(http://www.oschina.net/p/log_monitor)

(2) 源码编译:make

之后进行参数配置,可以拷贝现有的,各个参数说明如下:
```
log_file=/home/liao/programs/nginx/logs/access.log
log_reg=\[(.*) \+0800\]
time_format=%d/%b/%Y:%H:%M:%S
retain_seconds=3600
http_port=3344
log_level=INFO
```
     log_file: 需要监控的日志的路径
     
     log_reg: 匹配的日期及耗时信息的正则表达式(需要加括号), 第一个括号匹配的是时间, 第二个是耗时
     
     time_format: 日志中日期的格式
     
     retain_seconds: 统计数据保留时间
     
     http_port: 用来通过页面展示统计数据的端口
     
     log_level: log monitor的日志级别

### 启动

    ./bin/log_monitor [conf_file_path]

### 查看
    (1) 在控制台就可以看到系统实时的qps以及每一秒中接口的平均耗时情况

    (2) 通过浏览器访问 http://${ip}:{http_port}/show.html

### Now we have a new simple tool for stat qps: lm    
    How to use? 
    lm -s <file> or tail -f <file>|grep --line-buffered <pattern>|lm -s
