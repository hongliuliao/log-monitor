start_port=3450
server_num=5

i=0
while [ $i -lt $server_num ]
do
port=$(($start_port+$i))
echo 'start server on port:'$port

./bin/statistic_http_server $port &

i=$(($i+1))
done
