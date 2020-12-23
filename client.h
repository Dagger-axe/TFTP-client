typedef unsigned short ushort;
typedef long long ll;

/* TFTP包类型定义 */
#define TFTP_RRQ     1   //读文件请求
#define TFTP_WRQ     2   //写文件请求
#define TFTP_DATA    3   //文件数据包
#define TFTP_ACK     4   //回应包
#define TFTP_ERROR   5   //错误信息包

/* TFTP数据包结构体 */
#define DATA_SIZE 512
typedef struct TFTP_PACKET {
    ushort opcode;          //数据包类型
    union {
        ushort block;       //数据块编号
        ushort error_code;  //错误编号
        char filename[2];   //filename的前两个字节
    };
    /* RRQ和WRQ可变长filename的剩余部分及传输模式字段；
     * 数据包的data字段；
     * ACK包的空字段；
     * 错误包的错误信息字段
     */
    char data[DATA_SIZE];
} TFTP_PACKET;
#define TFTP_TIMEOUT 1000
#define TFTP_ERROR_TIMEOUT 10000
#define TFTP_END_SIZE 516
#define TFTP_FILE_SIZE 512
#define FLUSH_TIME 1000

/* 文件传输模式 */
#define NETASCII 0
#define OCTET    1
#define AMODE    "netascii"
#define OMODE    "octet"

/* server设置 */
#define SERVER_PORT_DEFAULT 69

/* 函数声明 */
void start_client(ushort opcode, ushort mode, char *filename, char *ip_addr);
void download(ushort mode, char *filename, char *ip_addr);  //下载，读
void upload(ushort mode, char *filename, char *ip_addr);  //上传，写