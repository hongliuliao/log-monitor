/*
 * flow_server.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <sstream>
#include "simple_log.h"
#include "epoll_socket.h"
#include "flow_server.h"

class FlowEpollWatcher : public EpollSocketWatcher {
public:
	FlowHandler *handler;

	virtual int on_accept(EpollContext &epoll_context) {
		return 0;
	};

    virtual int on_readable(int &epollfd, epoll_event &event) {
        EpollContext *epoll_context = (EpollContext *) event.data.ptr;
        int fd = epoll_context->fd;

        int buffer_size = SS_READ_BUFFER_SIZE;
        char read_buffer[buffer_size];
        memset(read_buffer, 0, buffer_size);

        int read_size = recv(fd, read_buffer, buffer_size, 0);
        if (read_size == -1 && errno == EINTR) {
            return READ_CONTINUE; 
        } 
        if (read_size == -1 /* io err*/|| read_size == 0 /* close */) {
            return READ_CLOSE;
        }
		return handler->do_handle(read_buffer, read_size);
    }

	virtual int on_writeable(EpollContext &epoll_context) {
		if (send(epoll_context.fd, "OK", 2, 0) == -1) {
			perror("send");
		}
		return 0;
	};

	virtual int on_close(EpollContext &epoll_context) {
		return 0;
	};
};

int FlowServer::start(int port, FlowHandler *handler, int backlog) {

	FlowEpollWatcher epoll_watcher;
	epoll_watcher.handler = handler;

	EpollSocket e_socket;
    e_socket.set_port(port);
    e_socket.set_watcher(&epoll_watcher);
    e_socket.set_backlog(backlog);
    e_socket.set_max_events(backlog);
	e_socket.start_epoll();

	return 0;
}



