typedef unsigned short ushort;

/* 错误信息码 */
#define ERR_NOT_DEF     0   //未定义 Not defined, see error message
#define ERR_NOT_FD      1   //文件未找到 File not found
#define ERR_ACC_VLT     2   //访问被拒绝 Access violation
#define ERR_DSK_FULL    3   //磁盘满或超出可分配空间 Disk full or allocation exceeded
#define ERR_ILG         4   //非法的 TFTP 操作 Illegal TFTP operation
#define ERR_UNKNOWN     5   //未知的传输 ID Unknown transfer ID
#define ERR_EXT         6   //文件已经存在 File already exists
#define ERR_NO_USR      7   //没有该用户 No such user

#define ERR_SOCK        10  //套接字创建失败
#define ERR_CREATE_FILE 11  //下载时本地创建同名文件失败
#define ERR_ERR_TMOUT   12  //回传的ERROR包丢失
#define ERR_NO_FILE     13  //本地没有该指定文件
#define ERR_SOCKET_ERR  14  //SOCKET_ERROR
#define ERR_NETASCII    15  //不可用netascii模式传输
#define ERR_TIMEOUT     16  //回收的包超时太多

void print_help();
void print_error(ushort err_type);
void print_begin(char *filename, char *ip_addr, int mode);
void print_speed(int snd, int rcv, int clk);
void print_size(int file, int snd, int rcv, int lost);
void print_result(int tm, double bps);