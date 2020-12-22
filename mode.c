#include "client.h"
#include "print.h"
#include "mode.h"

int is_netascii(char *filename) {  //读取该文件
    int ch, lch;
    FILE *pf = fopen("tempfile", "wb");
    FILE *pfile = fopen(filename, "rb");
    if (!pfile || !pf){
        print_error(ERR_NO_FILE);
        fclose(pfile);
        fclose(pf);
        return;
    }
    ch = fgetc(pfile);
    while (1) {
        //printf("file in while\n");
        lch = ch;
        ch = fgetc(pfile);
        if (lch >= 0x20 && lch <= 0x7E) fputc(lch, pf);
        else if (lch == 0) fputc(lch, pf);
        else if (lch >= 0x0F0 && lch <= 0x0FD) fputc(lch, pf);
        else if (lch == 0x0D) {  //当前为CR
            if (ch != 0 && ch != 0xA) {
                fputc(lch, pf);
                fputc(0, pf);
            }
        }
        else if (lch != 0x0D && ch == 0xA) {  //当前非CR，但下一个是LF
            fputc(lch, pf);
            fputc(0x0D, pf);
        }
        else if (lch >= 0x7 && lch <= 0x0D) fputc(lch, pf);
        else {  //非法情况
            fclose(pfile);
            fclose(pf);
            return RET_ERROR;
        }
        if (ch == EOF) break;
    }    
    fclose(pfile);
    fclose(pf);
    return RET_OK;
}