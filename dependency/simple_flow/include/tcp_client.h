/*
 * tcp_client.h
 *
 *  Created on: Nov 11, 2014
 *      Author: liao
 */

#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class TcpClient {
protected:
	int setNonblocking();
	int _sockfd;
	timeval _conn_timeout;
	timeval _read_timeout;
public:

	virtual int create_socket(timeval &conn_timeout, timeval &read_timeout);

	virtual int connect_socket(std::string ip, int port);

	virtual int connect_noblock_socket(std::string ip, int port);

	virtual int write_bytes(char *buffer, int size);

	virtual int read_bytes(char *buffer, int size);

	virtual int close_socket();

};

#endif /* TCP_CLIENT_H_ */
