### NETASCII

参考`RFC764`对该模式有如下要求：

+ 允许使用的字符集
  + `0x7`至`0x0D`的控制字符
  + `0x20`至`0x7E`的可打印字符
  + `NUL(0x0)`
  + `0xF0`至`0xFD`的`TELNET`命令可用字符

+ 要求`CR`后仅能为`CR LF`或`CR NUL`
+ 要求换行符仅能为`CR LF`

### 参考资料

[1]. RFC 764，https://www.rfc-editor.org/rfc/rfc764.html