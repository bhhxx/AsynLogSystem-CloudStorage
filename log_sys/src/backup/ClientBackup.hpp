#pragma once
#include <cstring>
#include <string>
#include <iostream>
#include <sys/socket.h> // for socket
#include <netinet/in.h> // for sockaddr
#include <arpa/inet.h>
#include "../Util.hpp"

extern asynlog::Util::JsonData *conf_data;

void start_log_backup(const std::string &msg) {
    // init socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << __FILE__ << __LINE__ << "socket error: " << strerror(errno) << std::endl;
        perror(NULL);
    }
    
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(conf_data->backup_port);
    inet_aton(conf_data->backup_addr.c_str(), &(server.sin_addr));

    // connect
    int retry = 5;
    while (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) { // here assign ip and port for client, which will be known by server
        std::cout << "Connect Retry! Times remain: " << retry-- << std::endl;
        if (retry <= 0) {
            std::cout << __FILE__ << __LINE__ << "connect error : " << strerror(errno) << std::endl;
            close(sock);
            perror(NULL);
            return;
        }
    }

    // send msg
    if (write(sock, msg.c_str(), msg.size()) == -1) {
        std::cout << __FILE__ << __LINE__ << "send to server error : " << strerror(errno) << std::endl;
        perror(NULL);
    }
    close(sock);
}