#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"

//枚举方式，定义消息类型
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,  //注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,  //注册回复
    ENUM_MSG_TYPE_MAX = 0x00ffffff, //uint最大值，0xfffffff

};


// 设计协议数据单元格式
struct PDU
{
    uint uiPDULen;       // 总的协议数据单元大小
    uint uiMsgType;      // 消息类型
    char caData[64];     // 其他数据
    uint uiMsgLen;       // 实际消息长度
    int caMsg[];         // 实际消息，主要通过caMsg访问消息数据
};

PDU *mkPDU(uint uiMsgLen); // 创建PDU，uiMsglen是可变的，总大小可有其计算得到


#endif // PROTOCOL_H
