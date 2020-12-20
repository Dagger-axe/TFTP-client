#include <stdio.h>
#include "print.h"

/* Usage instructions */
void print_help() {
    printf("Usage:\ntftp [-h] [type] [filepath] [IP]\n\n");
    printf("-h\tShows this usage instructions\n");
    printf("Type:\n");
    printf("\t-ra\tRequests a read in NETASCII mode\n");
    printf("\t-ro\tRequests a read in OCTET mode\n");
    printf("\t-wa\tRequests a write in NETASSCII mode\n");
    printf("\t-wo\tRequests a write in OCTET mode\n");
    printf("filepath\tPath to file to read or write\n");
    printf("IP\tTarget IP Address\n");
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
    else printf("Unknown error.\n");
}