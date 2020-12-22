#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include "client.h"
#include "print.h"

const int TFTP_PKT_SIZE = sizeof(TFTP_PACKET);
int sockfd;  //套接字句柄
struct sockaddr_in server;
struct sockaddr *server_ptr = (struct sockaddr*)&server;  //要求强制转换成sockaddr结构体后进入函数
int server_len = sizeof(struct sockaddr_in);  //结构体sockaddr_in的大小
int rcv_timeout = TFTP_TIMEOUT;
int snd_timeout = TFTP_TIMEOUT;

void start_client(ushort opcode, ushort mode, char *filename, char *ip_addr) {
    /* 创建套接字 */
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { print_error(ERR_SOCK); return; }
    server.sin_family = AF_INET;                    //地址族
    server.sin_addr.s_addr = inet_addr(ip_addr);    //IP地址
    server.sin_port = htons(SERVER_PORT_DEFAULT);   //端口号
    /* 设置TIMEOUT时间限制 */
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rcv_timeout, sizeof(int));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&snd_timeout, sizeof(int));

    if (opcode == TFTP_RRQ) download(mode, filename, ip_addr);
    else if (opcode == TFTP_WRQ) upload(mode, filename, ip_addr);
}

void download(ushort mode, char *filename, char *ip_addr) {
    print_begin(filename, ip_addr, mode);
    /* 创建本地同名文件准备写操作 */
    FILE *pfile = fopen(filename, "wb");
    if (!pfile) {
        print_error(ERR_CREATE_FILE);
        fclose(pfile);
        return;
    }
    /* 记录日志 */
    FILE *plog = fopen("logfile.log", "wb");
    /* 记录传输时间及文件大小的变量声明 */
    ll rcv_bytes = 0;
    clock_t clk_start = clock();
    /* send request 创建首个用户连接的RRQ数据包 */
    TFTP_PACKET snd_pkt;  //TFTP发送的数据包
    snd_pkt.opcode = htons(TFTP_RRQ);  //TFTP包类型
    int fok;
    if (mode == NETASCII) fok = sprintf(snd_pkt.filename, "%s%c%s%c", filename, 0, AMODE, 0);
    else if (mode == OCTET) fok = sprintf(snd_pkt.filename, "%s%c%s%c", filename, 0, OMODE, 0);
    /* receive data 接收应答的数据 */
    TFTP_PACKET rcv_pkt;  //接收的数据包
    ushort block = 1;
    int snd_size = 0, rcv_size = 0, lost_size = 0, file_size = 0;
    int snd_ret = 0, rcv_ret = 0;
    int temp_snd = 0, temp_rcv = 0, temp_snd2 = 0;  //记录flush时间段内的传输与接收字节数
    clock_t rcv_clk = clock(), flush_snd_clk = clock(), flush_rcv_clk = clock();
    while (1) {
        snd_ret = sendto(sockfd, &snd_pkt, fok + 4, 0, server_ptr, server_len);  //发送请求
        snd_size++;
        if (snd_ret == SOCKET_ERROR) {
            fprintf(plog, "[ERROR] SOCKET ERROR.\n");
            print_error(ERR_SOCKET_ERR);
            fclose(pfile); fclose(plog);
            return;
        }
        if (snd_pkt.opcode == TFTP_RRQ) fprintf(plog, "[INFO] Send a RRQ packet.\n");  //记录日志
        else fprintf(plog, "[INFO] Send a ACK packet of block %d.\n", block);

        if (clock() - flush_snd_clk > FLUSH_TIME) {  //更新实时流量器
            if (snd_ret > 0) temp_snd += snd_ret;            
            temp_snd2 = temp_snd;
            print_snd_speed(temp_snd, clock() - flush_snd_clk);
            print_rcv_speed(temp_rcv, clock() - flush_snd_clk);
            temp_snd = 0;
            flush_snd_clk = clock();
        }
        else if (snd_ret > 0) {
            temp_snd += snd_ret;
            temp_snd2 += snd_ret;
        }

        rcv_ret = recvfrom(sockfd, &rcv_pkt, TFTP_PKT_SIZE, 0, server_ptr, &server_len);  //尝试接收包
        if (rcv_ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {  //重传
                lost_size++;                
                fprintf(plog, "[WARNING] Timeout retransmission.\n");
                continue;
            }
            else {
                print_error(ERR_SOCKET_ERR);
                fclose(pfile); fclose(plog);
                return;
            }
        }        
        rcv_size++;
        rcv_clk = clock();

        if (clock() - flush_rcv_clk > FLUSH_TIME) {  //更新实时流量器
            if (rcv_ret > 0) temp_rcv += rcv_ret;
            print_snd_speed(temp_snd2, clock() - flush_rcv_clk);
            print_rcv_speed(temp_rcv, clock() - flush_rcv_clk);
            temp_rcv = 0;
            temp_snd2 = 0;
            flush_rcv_clk = clock();
        }
        else if (rcv_ret > 0) temp_rcv += rcv_ret;

        if (rcv_ret > 0) rcv_bytes += rcv_ret;  //增加接收的数据长度
        if (rcv_ret > 0 && rcv_ret < 4) {  //bad packet，重传请求
            fprintf(plog, "[WARNING] Bad packet.\n");
            continue;
        }
        else if (rcv_ret >= 4 && rcv_pkt.opcode == htons(TFTP_ERROR)) {  //TFTP错误
            fprintf(plog, "[ERROR] %s.\n", rcv_pkt.data);
            printf("[ERROR]***%s.\n", rcv_pkt.data);
            fclose(pfile); fclose(plog);
            return;
        }
        else if (rcv_ret >= 4 && rcv_pkt.opcode == htons(TFTP_DATA) && rcv_pkt.block == htons(block)) {
            if (rcv_ret < TFTP_END_SIZE) {  //当是最后一个data包时，记录并退出
                fprintf(plog, "[INFO] Receive a DATA packet of block %d.\n", block);
                fwrite(rcv_pkt.data, 1, rcv_ret - 4, pfile);
                file_size += rcv_ret - 4;
                double bps = (double)rcv_bytes / (double)(clock() - clk_start);
                print_size(file_size, snd_size, rcv_size, lost_size);
                print_result(clock() - clk_start, bps);
                fprintf(plog, "[INFO] Successfully read.\n");
                fclose(pfile); fclose(plog);
                return;
            }
            //成功接收一个数据，存储并发送ACK
            fprintf(plog, "[INFO] Received a DATA packet of block %d.\n", block);
            fwrite(rcv_pkt.data, 1, rcv_ret - 4, pfile);
            file_size += rcv_ret - 4;
            fok = 0;
            snd_pkt.opcode = htons(TFTP_ACK);
            snd_pkt.block = rcv_pkt.block;
            block++;
            continue;
        }
        else if (clock() - rcv_clk >= TFTP_ERROR_TIMEOUT) {  //回传的ERROR包丢失
            fprintf(plog, "[ERROR] ERROR packet lost.\n");
            print_error(ERR_ERR_TMOUT);
            fclose(pfile); fclose(plog);
            return;
        }
    } 
}

void upload(ushort mode, char *filename, char *ip_addr) {
    print_begin(filename, ip_addr, mode);
    /* 查找本地文件 */
    FILE *pfile = fopen(filename, "rb");
    if (!pfile){
        print_error(ERR_NO_FILE);
        fclose(pfile);
        return;
    }
    /* 记录日志 */
    FILE *plog = fopen("logfile.log", "wb");
    /* 创建计时器和传输量 */
    ll snd_bytes = 0;
    clock_t clk_start = clock();
    /* send request WRQ */
    TFTP_PACKET snd_pkt;  //TFTP发送的数据包
    snd_pkt.opcode = htons(TFTP_WRQ);  //TFTP包类型
    int fok;
    if (mode == NETASCII) fok = sprintf(snd_pkt.filename, "%s%c%s%c", filename, 0, AMODE, 0);
    else if (mode == OCTET) fok = sprintf(snd_pkt.filename, "%s%c%s%c", filename, 0, OMODE, 0);
    /* receive data 接收应答的数据 */
    TFTP_PACKET rcv_pkt;  //接收的数据包
    ushort block = 0;
    int snd_size = 0, rcv_size = 0, lost_size = 0, file_size = 0;
    int snd_ret = 0, rcv_ret = 0;
    int temp_snd = 0, temp_rcv = 0, temp_snd2 = 0;  //flush时间段内发送和接收的字节数
    clock_t rcv_clk = 0, flush_snd_clk = clock(), flush_rcv_clk = clock();
    while (1) {
        snd_ret = sendto(sockfd, &snd_pkt, fok + 4, 0, server_ptr, server_len);  //发送数据
        snd_size++;
        if (snd_ret == SOCKET_ERROR) {
            fprintf(plog, "[ERROR] SOCKET ERROR.\n");
            print_error(ERR_SOCKET_ERR);
            fclose(pfile); fclose(plog);
            return;
        } 
        snd_bytes += snd_ret;
        if (snd_pkt.opcode == TFTP_WRQ) fprintf(plog, "[INFO] Send a WRQ packet.\n");
        else fprintf(plog, "[INFO] Send a DATA packet of block %d.\n", block);

        if (clock() - flush_snd_clk > FLUSH_TIME) {  //更新实时流量器
            if (snd_ret > 0) temp_snd += snd_ret;            
            temp_snd2 = temp_snd;
            print_snd_speed(temp_snd, clock() - flush_snd_clk);
            print_rcv_speed(temp_rcv, clock() - flush_snd_clk);
            temp_snd = 0;
            flush_snd_clk = clock();
        }
        else if (snd_ret > 0) {
            temp_snd += snd_ret;
            temp_snd2 += snd_ret;
        }

        rcv_ret = recvfrom(sockfd, &rcv_pkt, TFTP_PKT_SIZE, 0, server_ptr, &server_len);  //尝试接收包
        if (rcv_ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {  //重传
                lost_size++;                
                fprintf(plog, "[WARNING] Timeout retransmission.\n");
                continue;
            }
            else {
                print_error(ERR_SOCKET_ERR);
                fclose(pfile); fclose(plog);
                return;
            }
        }
        rcv_size++;
        rcv_clk = clock();

        if (clock() - flush_rcv_clk > FLUSH_TIME) {  //更新实时流量器
            if (rcv_ret > 0) temp_rcv += rcv_ret;
            print_snd_speed(temp_snd2, clock() - flush_rcv_clk);
            print_rcv_speed(temp_rcv, clock() - flush_rcv_clk);
            temp_rcv = 0;
            temp_snd2 = 0;
            flush_rcv_clk = clock();
        }
        else if (rcv_ret > 0) temp_rcv += rcv_ret;

        if (rcv_ret > 0 && rcv_ret < 4) {  //bad packet，重传请求
            fprintf(plog, "[WARNING] Bad packet.\n");
            continue;
        }
        else if (rcv_ret >= 4 && rcv_pkt.opcode == htons(TFTP_ERROR)) {  //TFTP错误
            fprintf(plog, "[ERROR] %s.\n", rcv_pkt.data);
            printf("[ERROR]***%s.\n", rcv_pkt.data);
            fclose(pfile); fclose(plog);
            return;
        }
        else if (rcv_ret == 4 && rcv_pkt.opcode == htons(TFTP_ACK) && rcv_pkt.block == htons(block)) {
            fok = fread(snd_pkt.data, 1, TFTP_FILE_SIZE, pfile);
            file_size += fok;
            fprintf(plog, "[INFO] Received a ACK packet of block %d.\n", block);
            //成功接收ACK并继续传数据DATA
            snd_pkt.opcode = htons(TFTP_DATA);
            block++;
            snd_pkt.block = htons(block);           
            if (fok != TFTP_FILE_SIZE) {  //当是最后一个data包时，记录并退出
                snd_ret = sendto(sockfd, &snd_pkt, fok + 4, 0, server_ptr, server_len);
                snd_size++;
                if (snd_ret == SOCKET_ERROR) {
                    fprintf(plog, "[ERROR] SOCKET ERROR.\n");
                    print_error(ERR_SOCKET_ERR);
                    fclose(pfile); fclose(plog);
                    return;
                } 
                snd_bytes += snd_ret;
                fprintf(plog, "[INFO] Send a DATA packet of block %d.\n", block);
                double bps = (double)snd_bytes / (double)(clock() - clk_start);
                print_size(file_size, snd_size, rcv_size, lost_size);
                print_result(clock() - clk_start, bps);
                fprintf(plog, "Successfully write.\n");
                fclose(pfile); fclose(plog);
                return;
            }
            continue;
        }
        else if (clock() - rcv_clk >= TFTP_ERROR_TIMEOUT) {  //回传的ERROR包丢失
            fprintf(plog, "[ERROR] ERROR packet lost.\n");
            print_error(ERR_ERR_TMOUT);
            fclose(pfile); fclose(plog);
            return;
        }        
    }
}