log_monitor
===========
How to use? 
```
tail -f <file>|grep --line-buffered <pattern>|lm -s
```
Output format:
```
log_time: 2019-10-28 10:36:18, QPS:10, avg_cost_time:22, max_cost_time:222
log_time: 2019-10-28 10:36:20, QPS:1, avg_cost_time:222, max_cost_time:222
log_time: 2019-10-28 10:36:21, QPS:2, avg_cost_time:111, max_cost_time:222
log_time: 2019-10-28 10:36:22, QPS:2, avg_cost_time:166, max_cost_time:333
log_time: 2019-10-28 10:36:24, QPS:2, avg_cost_time:166, max_cost_time:333
log_time: 2019-10-28 10:36:25, QPS:1, avg_cost_time:111, max_cost_time:111
log_time: 2019-10-28 10:36:32, QPS:1, avg_cost_time:111, max_cost_time:111
```

## Function list	
  * Stat for qps
  * Stat for avg/max cost time(if filter by number)
## Compile
  * make

