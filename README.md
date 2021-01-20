<p align="right"><a href="#Chinese">中文版README</a></p>

### Abstract

&emsp;&emsp;A `TFTP` client program, to achieve two modes of file upload and download:

+ Two different transmission modes `NETASCII` and `OCTET`
+ Adequate and targeted `ERROR` message
+ Real-time traffic monitoring and display
+ Important transmission result information, including: file size, number of sent packets, number of received packets, number of lost packets and throughput, etc
+ Generate logfile `logfile.log`, including: sending, receiving, timeout retransmission and other behavior records

### Compile

```bash
gcc main.c client.c print.c mode.c -o tftp -lwsock32 -w
```

### How to use

```
tftp [-h] <type> <filepath> <IP>
```

### Overall architecture and implementation

#### main.c

&emsp;&emsp;Used to analyze the user input instructions, and deal different instructions with different responses. When the instruction error, prompt the error information and exit, otherwise, upload or download will be performed separately for both modes. `Usage` information is as follows:

```
Usage:    tftp [-h] <type> <filepath> <IP>

-h        Shows this usage instructions
type:
   -ra    Requests a read in NETASCII mode
   -ro    Requests a read in OCTET mode
   -wa    Requests a write in NETASSCII mode
   -wo    Requests a write in OCTET mode
filepath  Path to file to read or write
IP        Target IP Address
```

#### print.c

&emsp;&emsp;Used to output `Usage`, error message, real-time traffic information, transmission speed and final results to the terminal.

#### mode.c

&emsp;&emsp;Used for `NETASCII` mode verification and conversion: when local file upload, first check whether the current file conforms to the `NETASCII` mode transmission standard, if so, according to the standard for conversion, transfer and delete temporary files after conversion, otherwise, it prompts the user to be unable to transfer in `NETASCII` mode. When downloading files, check the downloaded files to determine whether they conform to `NETASCII` mode. If so, convert them according to the rules. After conversion, delete the downloaded original files and rename the converted temporary files to the original file names. Otherwise, it prompts the user to be unable to transfer in `NETASCII` mode, and deletes the downloaded original file and temporary file.

##### NETASCII

&emsp;&emsp;Reference `RFC764` has the following requirements for this mode:
+ Allowed character set
  + Control characters from  `0x7` to `0x0D`
  + Printable characters from `0x20` to `0x7E`
  + `NUL(0x0)`
  + Available `TELNET` command characters from `0xF0` to `0xFD` 
+ After `CR`, it can only be `CR LF` or `CR NUL`
+ Newline character to be only `CR LF`

#### client.h

&emsp;&emsp;In this file, the structure that `TFTP` protocol needs is defined, which is the message format of `TFTP` protocol. The details are as follows:

```c
#define DATA_SIZE 512
typedef struct TFTP_PACKET {
    ushort opcode;          //packet type
    union {
        ushort block;       //packet number
        ushort error_code;  //error number
        char filename[2];   //the first two bytes of filename
    };
    /* RRQ and WRQ variable length filename remainder and transfer mode fields
     * Data field of the packet
     * ACK packet empty field
     * Error message field for the error package
     */
    char data[DATA_SIZE];
} TFTP_PACKET;
```

&emsp;&emsp;Bytes 3-4:  for `ACK` and `DATA` packets, record the DATA block number. For `ERROR` packets, log the ERROR number. For `RRQ` and `WRQ` packages, partial transfer file name information is recorded. Therefore, this field is implemented using the `union` structure.

#### client.c

&emsp;&emsp;This file is the main file of `TFTP` protocol implementation.

##### Initialize: start_client()

&emsp;&emsp;This function is used to perform initialization for an upload or download task. Generates the corresponding socket according to the user specified `IP` address, and defines the socket receive timeout. In view of the two different working modes to make different responses to control the temporary file.

##### Upload: Upload ()

&emsp;&emsp;This function is used to complete the file upload. Opens the specified file or temporary file according to different modes. Build a packet of type `WRQ` and send it. According to the returned packet, determine the updated transmission port number and record it. Use the `recvfrom()` function in the transmission queue to try to fetch packets from the updated transmission port. Depending on the type of packet returned, different responses will be made, including: bad packet retransmission, timeout retransmission, `ERROR` packet resolution, update of `ACK` packet uploaded data, and interrupt caused by returned `ERROR` packet. When the file transfer is complete, output the comprehensive transfer result information.

&emsp;&emsp;At the same time, real-time traffic display is carried out, and the traffic is updated once every period of time (the author sets it as 1s).

##### Download: Download ()

&emsp;&emsp;This function is used to complete the download of the file. Opens the specified file. The `RRQ` package is constructed and sent, and the updated transmission port number is determined according to the returned packet and recorded. Use the`'recvfrom()` function in the transmission queue to try to fetch packets from the updated transmission port. Depending on the type of packet returned, different responses will be made, including: bad packet retransmission, timeout retransmission, `ERROR` packet parsing, writing `DATA` to a file and updating `ACK` packet DATA, and interrupts caused by returned `ERROR` packets. When the last packet transmission is completed, the downloaded files are converted according to different modes, and failure information is output when the conversion fails. Convert successfully outputs the comprehensive transmission result information.

### Test

#### Platform

&emsp;&emsp;In the unstable network environment, use `xinetd`, `TFTP` and `tftpd-hpa` to complete the construction of the test platform on `Ubuntu`, and use `tc` to complete the flow control, as follows:

```bash
tc qdisc add dev wlp3s0 root netem loss 30%
tc qdisc add dev wlp3s0 root netem delay 100ms 10ms 30%
```

#### Result

```
E:\network\lab\tiny-TFTP>tftp -ro temp3 10.12.180.159

File name:           temp3
IP address:          10.12.180.159
Transmission mode:   OCTET

Send packet:40   Receive packet:29   Lost packet:11
File total size:     14762B
Total time:          16305ms
Transmission speed:  0.91Kbps
```

### Reference

[1]. RFC 764，https://www.rfc-editor.org/rfc/rfc764.html



<p align="center"><a id="Chinese"> </a></p>

### 概述

&emsp;&emsp;一个`TFTP`客户端程序，实现两种模式的文件的上传及下载：

+ 两种不同的传输模式`NETASCII`和`OCTET` 
+ 充分的、具有针对性的`ERROR`报错信息
+ 实现对实时流量的监控和显示
+ 重要的传输结果信息，包括：文件大小、发包个数、收包个数、丢包个数以及吞吐量等
+ 生成日志文件`logfile.log`，包括：发包、收包、超时重传等行为的记录

### 编译

```bash
gcc main.c client.c print.c mode.c -o tftp -lwsock32 -w
```

### 使用

```
tftp [-h] <type> <filepath> <IP>
```

### 整体架构及实现

#### main.c

&emsp;&emsp;用于分析用户输入的指令，并对不同的指令进行不同的响应。当指令错误时，提示用户错误并退出；否则针对两种模式分别执行上传或下载。`Usage`信息如下：

```
Usage:    tftp [-h] <type> <filepath> <IP>

-h        Shows this usage instructions
type:
   -ra    Requests a read in NETASCII mode
   -ro    Requests a read in OCTET mode
   -wa    Requests a write in NETASSCII mode
   -wo    Requests a write in OCTET mode
filepath  Path to file to read or write
IP        Target IP Address
```

#### print.c

&emsp;&emsp;用于向终端输出`Usage`、错误提示信息、实时流量信息、传输速度以及最终结果等。

#### mode.c

&emsp;&emsp;用于进行`NETASCII`模式的验证及转换：当进行本地文件上传时，首先检查当前文件是否符合以`NETASCII`模式传输的标准，若符合则按照标准进行转换，转换后传输并删除临时文件；否则提示用户无法以`NETASCII`模式传输。当进行文件下载时，对下载完成的文件进行检查，判断其是否符合`NETASCII`模式，若符合则按照规则进行转换，转换后删除下载的原文件并对转换后的临时文件重命名为原文件名；否则提示用户无法以`NETASCII`模式传输，并删除下载的原文件及临时文件。

##### NETASCII

&emsp;&emsp;参考`RFC764`对该模式有如下要求：

+ 允许使用的字符集
  + `0x7`至`0x0D`的控制字符
  + `0x20`至`0x7E`的可打印字符
  + `NUL(0x0)`
  + `0xF0`至`0xFD`的`TELNET`命令可用字符
+ 要求`CR`后仅能为`CR LF`或`CR NUL`
+ 要求换行符仅能为`CR LF`

#### client.h

&emsp;&emsp;在该文件中定义了`TFTP`协议所需要的结构体，即`TFTP`协议的报文格式。具体如下：

```c
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
```

&emsp;&emsp;第3至4字节：对于`ACK`及`DATA`包，记录数据块编号；对于`ERROR`包，记录错误编号；对于`RRQ`及`WRQ`包，记录部分传输文件名信息。因此该字段使用`union`结构实现。

#### client.c

&emsp;&emsp;该文件是`TFTP`协议实现的主体文件。

##### 初始化：start_client()

&emsp;&emsp;该函数用于完成对上传或下载任务的初始化。根据用户指定的`IP`地址产生相应的套接字，并对套接字的接收超时进行定义。针对两种不同的工作模式做出不同的响应，来控制临时文件的去留。

##### 上传：upload()

&emsp;&emsp;该函数用于完成文件的上传。根据不同模式打开指定的文件或临时文件。构建`WRQ`类型的数据包并发送，根据返回的数据包确定更新后的传输端口号并记录。使用`recvfrom()`函数在传输队列中尝试取出更新后传输端口传输的数据包。根据该回传的数据包类型做出不同响应，包括：坏包重传、超时重传、`ERROR`包解析、对`ACK`包进行上传数据的更新以及回传的`ERROR`包引发的中断。当文件传输完成后，输出综合的传输结果信息。

&emsp;&emsp;同时进行实时流量显示，每一段时间（笔者设置为1s）更新一次流量。

##### 下载：download()

&emsp;&emsp;该函数用于完成文件的下载。打开指定文件。构建`RRQ`包并发送，根据返回的数据包确定更新后的传输端口号并记录。使用`recvfrom()`函数在传输队列中尝试取出更新后传输端口传输的数据包。根据该回传的数据包类型做出不同响应，包括：坏包重传、超时重传、`ERROR`包解析、将`DATA`包数据写入文件并更新`ACK`包数据以及回传的`ERROR`包引发的中断。当最后一个数据包传输完成后，根据不同的模式对下载的文件进行转换，转换失败输出失败信息；转换成功输出综合的传输结果信息。

### 测试

#### 平台环境

&emsp;&emsp;不稳定的网络环境，`Ubuntu`上使用`xinetd`、 `tftp`和`tftpd-hpa`完成测试平台的搭建，同时使用`tc`完成流量控制，具体为：

```bash
tc qdisc add dev wlp3s0 root netem loss 30%
tc qdisc add dev wlp3s0 root netem delay 100ms 10ms 30%
```

#### 结果

```
E:\计网\实验\tiny-TFTP>tftp -ro temp3 10.12.180.159

File name:           temp3
IP address:          10.12.180.159
Transmission mode:   OCTET

Send packet:40   Receive packet:29   Lost packet:11
File total size:     14762B
Total time:          16305ms
Transmission speed:  0.91Kbps
```

### 参考资料

[1]. RFC 764，https://www.rfc-editor.org/rfc/rfc764.html
