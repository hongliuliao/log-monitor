/*
 * tcp_client.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: liao
 */

#include <sys/fcntl.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tcp_client.h"

int TcpClient::create_socket(timeval &conn_timeout, timeval &read_timeout) {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        perror("ERROR opening socket");
        return -1;
    }
    _conn_timeout = conn_timeout;
    setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, &conn_timeout, sizeof(timeval)); // in linux kernel, connect timeout == send timeout
    _read_timeout = read_timeout;
    setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(timeval));
    return 0;
}

int TcpClient::setNonblocking() {
	int flags;
	int fd = _sockfd;

	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int TcpClient::connect_socket(std::string ip, int port) {
	struct sockaddr_in serv_addr;
	struct hostent *server;
	server = gethostbyname(ip.c_str());
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if (connect(_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		return -1;
	}
	return 0;
}

int TcpClient::connect_noblock_socket(std::string ip, int port) {
	int ret = this->connect_socket(ip, port);
	if(ret != 0) {
		return ret;
	}
	this->setNonblocking();
	return 0;
}

int TcpClient::write_bytes(char *buffer, int size) {
    return write(_sockfd, buffer, size);
}

int TcpClient::read_bytes(char *buffer, int size) {
    return read(_sockfd, buffer, size);
}

int TcpClient::close_socket() {
    return close(_sockfd);
}
