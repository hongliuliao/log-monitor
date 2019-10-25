/*
 * flow_utils.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: liao
 */
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "flow_utils.h"

std::string get_host_name() {
    char hname[128];
    gethostname(hname, sizeof(hname));
    return std::string(hname);
}

std::string get_local_ip() {
    std::string ip;
    struct ifaddrs * if_addr_struct = NULL;
    void * tmp_addr_ptr = NULL;

    getifaddrs(&if_addr_struct);

    while (if_addr_struct != NULL) {
        if (if_addr_struct->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmp_addr_ptr = &((struct sockaddr_in *) if_addr_struct->ifa_addr)->sin_addr;
            char address_buffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmp_addr_ptr, address_buffer, INET_ADDRSTRLEN);
            if (strncmp(if_addr_struct->ifa_name, "lo", 2) != 0) {
                ip = address_buffer;
            }
        }
        if_addr_struct = if_addr_struct->ifa_next;
    }
    return ip;
}
