log_monitor
===========
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
  * 轻量: 共219KB
  * 低侵入: 基于日志文件进行统计,无需修改原程序
## 使用说明
  * 编译: make

