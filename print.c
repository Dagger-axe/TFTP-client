#include <stdio.h>
#include "client.h"
#include "print.h"

/* Usage instructions */
void print_help() {
    printf("Usage:\t  tftp [-h] <type> <filepath> <IP>\n\n");
    printf("-h\t  Shows this usage instructions\n");
    printf("Type:\n");
    printf("\t  -ra\tRequests a read in NETASCII mode\n");
    printf("\t  -ro\tRequests a read in OCTET mode\n");
    printf("\t  -wa\tRequests a write in NETASSCII mode\n");
    printf("\t  -wo\tRequests a write in OCTET mode\n");
    printf("filepath  Path to file to read or write\n");
    printf("IP\t  Target IP Address\n");
}

/* 错误信息 */
void print_error(ushort error) {
    printf("[ERROR]***");
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
    else printf("Unknown error.\n");
}

void print_result(FILE *plog, char *filename, char *ip_addr, int mode, double bps) {
    printf("---------------RESULT---------------\n");
    printf("File name:           %s\n", filename);
    printf("IP address:          %s\n", ip_addr);
    if (mode == NETASCII)  printf("Transmission mode:   NETASCII\n");
    else printf("Transmission mode:   OCTET\n");
    printf("Transmission speed:  %.2lfbps\n", bps);
    printf("------------------------------------\n");
    
    fprintf(plog, "---------------RESULT---------------\n");
    fprintf(plog, "File name:           %s\n", filename);
    fprintf(plog, "IP address:          %s\n", ip_addr);
    if (mode == NETASCII)  fprintf(plog, "Transmission mode:   NETASCII\n");
    else fprintf(plog, "Transmission mode:   OCTET\n");
    fprintf(plog, "Transmission speed:  %.2lfbps\n", bps);
    fprintf(plog, "------------------------------------\n");
}