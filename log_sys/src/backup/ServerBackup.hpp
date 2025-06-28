#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>

using func_t = std::function<void(const std::string &)>;
const int backlog_times = 32;
class TCP_Server;

class ThreadData
{
public:
    ThreadData(int fd, const std::string &ip, const uint16_t &port, TCP_Server *ts) : sock(fd), client_ip(ip), client_port(port), ts_(ts){}

public:
    int sock;
    std::string client_ip;
    uint16_t client_port;
    TCP_Server *ts_;
};

class TCP_Server {
private:
    int listen_sock_;
    uint16_t port_;
    func_t func_;
public:
    TCP_Server(uint16_t port, func_t func) : port_(port), func_(func) {}

    void init_service() {
        // init socket
        listen_sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_sock_ == -1) {
            std::cout << __FILE__ << __LINE__ << "socket error: " << strerror(errno) << std::endl;
            perror(NULL);
        }
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = htons(port_);
        local.sin_addr.s_addr = htonl(INADDR_ANY);

        // bind socket with ip and port
        if (bind(listen_sock_, (struct sockaddr *)&local, sizeof(local)) < 0) {
            std::cout << __FILE__ << __LINE__ << "bind socket error"<< strerror(errno) << std::endl;
        }

        // listen
        if (listen(listen_sock_, backlog_times) < 0) {
            std::cout << __FILE__ << __LINE__ << "listen error" << strerror(errno) << std::endl;
        }
    }

    void service(int sock, const std::string&& client_info)
    {
        char buf[1024];

        int r_ret = read(sock, buf, sizeof(buf));
        if (r_ret ==-1) {
            std::cout << __FILE__ << __LINE__ << "read error" << strerror(errno) << std::endl;
            perror("NULL");
        } else if (r_ret > 0) {
            buf[r_ret] = 0;
            std::string tmp = buf;
            func_(client_info + tmp);
        }
    }
    void start_service() {
        while (true) {
            struct sockaddr_in client;
            socklen_t client_len = sizeof(client);
            int connfd = accept(listen_sock_, (struct sockaddr *)&client, &client_len);
            if (connfd < 0) {
                std::cout << __FILE__ << __LINE__ << "accept error" << strerror(errno) << std::endl;
                continue;
            }
            std::string client_ip = inet_ntoa(client.sin_addr);
            uint16_t client_port = ntohs(client.sin_port);

            pthread_t tid;
            ThreadData *td = new ThreadData(connfd, client_ip, client_port, this);
            pthread_create(&tid, nullptr, threadRoutine, td);
        }
    }
    static void *threadRoutine(void *args)
    {
        pthread_detach(pthread_self()); // 防止在start_service处阻塞

        ThreadData *td = static_cast<ThreadData *>(args);
        std::string client_info = td->client_ip + ":" + std::to_string(td->client_port);
        td->ts_->service(td->sock, move(client_info));
        close(td->sock);
        delete td;
        return nullptr;
    }
    ~TCP_Server()=default;
};