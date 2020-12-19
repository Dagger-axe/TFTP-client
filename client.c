#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include "client.h"
#include "print.h"

int sockfd;  //套接字句柄
struct sockaddr_in server;
struct sockaddr *server_ptr = (struct sockaddr*)&server;  //要求强制转换成sockaddr结构体后进入函数
int server_len = sizeof(struct sockaddr_in);  //结构体sockaddr_in的大小

void start_client(ushort opcode, ushort mode, char *filename, char *ip_addr) {
    /* 创建套接字 */
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) { print_error(ERR_SOCK); return; }
    server.sin_family = AF_INET;                    //地址族
    server.sin_addr.s_addr = inet_addr(ip_addr);    //IP地址
    server.sin_port = htons(SERVER_PORT_DEFAULT);   //端口号

    if (opcode == RRQ) download(mode, filename, ip_addr);
    else if (opcode == WRQ) upload(mode, filename, ip_addr);
}

void download(ushort mode, char *filename, char *ip_addr) {
    /* 创建本地同名文件准备写操作 */
    FILE *pfile = fopen(filename, "wb");
    if (!pfile) {
        print_error(ERR_CREATE_FILE);
        return;
    }
    /* 记录传输时间及文件大小的变量声明 */
    ll rcv_bytes = 0;
    clock_t clk_start = clock();
    /* send request 创建首个用户连接的RRQ数据包 */
    TFTP_PACKET snd_pkt;  //TFTP发送的数据包
    snd_pkt.opcode = htos(TFTP_RRQ);  //TFTP包类型
    if (mode == NETASCII) sprintf(snd_pkt.filename, "%s%c%s%c", filename, 0, AMODE, 0);
    else if (mode == OCTET) sprintf(snd_pkt.filename, "%s%c%s%c", filename, 0, OMODE, 0);
    /* receive data 接收第一个应答的数据 */
    TFTP_PACKET rcv_pkt;  //接收的数据包
    while (1) {
        clock_t temp_start = clock();
        sendto(sockfd, &snd_pkt, TFTP_PKT_SIZE, 0, server_ptr, server_len);  //发送请求
    download_try_receive:
        int ret = recvfrom(sockfd, &rcv_pkt, TFTP_PKT_SIZE, 0, server_ptr, &server_len);  //尝试接收包
        if (ret > 0) rcv_bytes += ret;  //增加接收的数据长度
        if (ret > 0 && ret < 4) continue;  //bad packet，重传请求
        else if (ret >= 4 && rcv_pkt.opcode == htons(TFTP_ERROR)) {  //TFTP错误
            print_error(rcv_pkt.error_code);
            return;
        }
        else if (ret >= 4 && rcv_pkt.opcode == htons(TFTP_DATA) && rcv_pkt.block == htons(1)) {
            //成功接收第一个数据，存储并发送ACK
            goto download_send_ACK;
        }
        else if (clock() - temp_start < TFTP_TIMEOUT) goto download_try_receive;
    }    
    /* 持续接收数据并发送ACK或重传 */
    while (1) {
        
    }
    recvfrom();
}

void upload(ushort mode, char *filename, char *ip_addr) {

}