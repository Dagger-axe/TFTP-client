#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "client.h"
#include "print.h"

#define RET_ERROR 0
#define RET_OK    1

int is_ip(char *ip){  //判断是否为合法IP地址
    if (inet_addr(ip) == INADDR_NONE) return RET_ERROR;
    else return RET_OK;
}

int main(int argc, char **argv) {
    int type, mode;  //写或读类型，netascii或octet模式
    if (argc != 2 && argc != 4) printf("[ERROR]***Invalid arguments provided. Use -h for help.\n");
    else if (!strcmp(argv[1], "-ra")) {
        type = TFTP_RRQ;
        mode = NETASCII;
    }
    else if (!strcmp(argv[1], "-ro")) {
        type = TFTP_RRQ;
        mode = OCTET;
    }
    else if (!strcmp(argv[1], "-wa")) {
        type = TFTP_WRQ;
        mode = NETASCII;
    }
    else if (!strcmp(argv[1], "-wo")) {
        type = TFTP_WRQ;
        mode = OCTET;
    }
    else if (!strcmp(argv[1], "-h")) print_help();
    else printf("[ERROR]***Invalid arguments provided. Use -h for help.\n");

    if (argc == 4) {
        if (is_ip(argv[3])) start_client(type, mode, argv[2], argv[3]);  //开启客户端
        else printf("[ERROR]***Invalid IP address.\n");
    }
    return 0;
}