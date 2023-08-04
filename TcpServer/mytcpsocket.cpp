#include "mytcpsocket.h"
#include <QDebug>

MyTcpSocket::MyTcpSocket()
{
    //自己封装的MyTcpSocket函数，可以自己产生的socket，自己本身来接收处理
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));
}

//处理注册请求并返回相应PDU
PDU* handleRegistRequest(PDU *pdu)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    strncpy(caName, pdu->caData, 32);
    strncpy(caPwd, pdu->caData + 32, 32 );
    qDebug() << pdu->uiMsgType << " " << caName << " " << caPwd;

    bool ret = DBoperate::getInstance().handleRegist(caName, caPwd);    //处理请求，插入数据库
    qDebug() << ret;
    // 响应客户端
    PDU *resPdu = mkPDU(0); // 响应消息
    resPdu -> uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    if(ret)
    {
        strcpy(resPdu -> caData, REGIST_OK);
    }
    else
    {
        strcpy(resPdu -> caData, REGIST_FAILED);
    }
    // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;

    return resPdu;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << this -> bytesAvailable();
    // 所处状态不是接收文件，接收到的是非文件传输的请求
    uint uiPDULen = 0;
    this -> read((char*)&uiPDULen, sizeof(uint)); // 先读取uint大小的数据，首个uint正是总数据大小
    uint uiMsgLen = uiPDULen - sizeof(PDU); // 实际消息大小，sizeof(PDU)只会计算结构体大小，而不是分配的大小
    PDU *pdu = mkPDU(uiMsgLen);
    this -> read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收剩余部分数据（第一个uint已读取）
    // qDebug() << pdu -> uiMsgType << ' ' << (char*)pdu -> caMsg; // 输出

    // 根据不同消息类型，执行不同操作
    PDU* resPdu = NULL; // 响应报文
    switch(pdu -> uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST: // 注册请求
    {
        resPdu = handleRegistRequest(pdu); // 请求处理
        break;
    }
    default:
        break;
    }

    // 响应客户端
    if(NULL != resPdu)
    {
        qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;
        this -> write((char*)resPdu, resPdu -> uiPDULen);
        // 释放空间
        free(resPdu);
        resPdu = NULL;
    }
    // 释放空间
    free(pdu);
    pdu = NULL;

//    qDebug() << caName << caPwd << pdu->uiMsgType;
}



