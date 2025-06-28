#include <string>
#include <iostream>
#include <sys/stat.h>
#include <cassert>
#include "ServerBackup.hpp"
#include <memory>
const std::string filename = "./logfile.log";

void usage(std::string procgress) {
    std::cout << "usage error:" << procgress << "port" << std::endl;
}

bool file_exist(const std::string &name) {
    struct stat exist;
    return (stat(name.c_str(), &exist) == 0);
}

void backup_log(const std::string &message) {
    FILE *fp = fopen(filename.c_str(), "ab");
    if (fp == NULL) {
        perror("fopen error: ");
        assert(false);
    }
    int write_byte = fwrite(message.c_str(), 1, message.size(), fp);
    if (write_byte != message.size()) {
        perror("fwrite error: ");
        assert(false);
    }
    fflush(fp);
    fclose(fp);
}

int main(int args, char *argv[])
{
    if (args != 2) {
        usage(argv[0]);
        perror("usage error");
        exit(-1);
    }

    uint16_t port = atoi(argv[1]);
    std::unique_ptr<TCP_Server> tcp(new TCP_Server(port, backup_log));

    tcp->init_service();
    tcp->start_service();

    return 0;
}