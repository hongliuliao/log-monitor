log_monitor
===========
### Now we have a new simple tool for stat qps: lm    
How to use? 
```
lm -s <file>
```
or
```
tail -f <file>|grep --line-buffered <pattern>|lm -s
```

## 功能介绍	
对qps以及接口的平均耗时进行统计
## 特点	
(1)轻量: 共219KB
(2)低侵入: 基于日志文件进行统计,无需修改原程序
## 使用说明
(1) 编译:make

## 参数配置(使用lm的可以忽略)
   
   | param_name     | param_desc                | example                          |
   | ----------     | ------------------------- | -------------------------------- |
   | log_file       | 需要监控的日志的路径        | /usr/local/nginx/logs/access.log |
   | log_reg        | 匹配日期及耗时正则(需加括号) | \\\[(.*) \\\+0800\\\]            |
   | time_format    | 日志中日期的格式            | %d/%b/%Y:%H:%M:%S|
   | retain_seconds | 统计数据保留时间            | 3600                             |
   | http_port      | 页面展示统计数据的端口       | 3344                             |
   | log_level      | log monitor的日志级别       | INFO                             |

### 启动(使用lm的可以忽略)

    ./bin/log_monitor [conf_file_path]

### 查看(使用lm的可以忽略)
    (1) 在控制台就可以看到系统实时的qps以及每一秒中接口的平均耗时情况

    (2) 通过浏览器访问 http://${ip}:{http_port}/show.html

