#include <stdio.h>
#include "client.h"
#include "print.h"

/* Usage instructions */
void print_help() {
    printf("Usage:\t  tftp [-h] <type> <filepath> <IP>\n\n");
    printf("-h\t  Shows this usage instructions\n");
    printf("type:\n");
    printf("   -ra\t  Requests a read in NETASCII mode\n");
    printf("   -ro\t  Requests a read in OCTET mode\n");
    printf("   -wa\t  Requests a write in NETASSCII mode\n");
    printf("   -wo\t  Requests a write in OCTET mode\n");
    printf("filepath  Path to file to read or write\n");
    printf("IP\t  Target IP Address\n");
}

/* 错误信息 */
void print_error(ushort error) {
    printf("\n[ERROR]***");
    if (error == ERR_NOT_DEF) printf("Not defined error, see error message.\n");
    else if (error == ERR_NOT_FD) printf("File not found.\n");
    else if (error == ERR_ACC_VLT) printf("Access violation.\n");
    else if (error == ERR_DSK_FULL) printf("Disk full or allocation exceeded.\n");
    else if (error == ERR_ILG) printf("Illegal TFTP operation.\n");
    else if (error == ERR_UNKNOWN) printf("Unknown transfer ID.\n");
    else if (error == ERR_EXT) printf("File already exists.\n");
    else if (error == ERR_NO_USR) printf("No such user.\n");
    else if (error == ERR_SOCK) printf("Server socker could not be created.\n");
    else if (error == ERR_CREATE_FILE) printf("Fail to create the file.\n");
    else if (error == ERR_ERR_TMOUT) printf("ERROR packet lost.\n");
    else if (error == ERR_NO_FILE) printf("No such local file.\n");
    else if (error == ERR_SOCKET_ERR) printf("SOCKET ERROR.\n");
    else if (error == ERR_NETASCII) printf("Can\'t send by NETASCII mode.\n");
    else printf("Unknown error.\n");
}

void print_begin(char *filename, char *ip_addr, int mode) {
    putchar('\n');
    printf("File name:           %s\n", filename);
    printf("IP address:          %s\n", ip_addr);
    if (mode == NETASCII)  printf("Transmission mode:   NETASCII\n");
    else printf("Transmission mode:   OCTET\n");
    putchar('\n');
}

void print_speed(int snd, int rcv, int clk) {
    if (snd < 0) snd = 0;
    if (rcv < 0) rcv = 0;
    if (clk < 1) clk = 1;
    double res_snd = (double)snd / (double)clk;
    double res_rcv = (double)rcv / (double)clk;
    printf("\rSend speed:%-6.2lfKbps      Receive speed:%-6.2lfKbps", res_snd, res_rcv);
}

void print_size(int file, int snd, int rcv, int lost) {
    printf("\rSend packet:%d   Receive packet:%d   Lost packet:%d   \n", snd, rcv, lost);
    printf("File total size:     %dB\n", file);
}

void print_result(int tm, double bps) {    
    printf("Total time:          %dms\n", tm);
    printf("Transmission speed:  %.2lfKbps\n", bps);
}