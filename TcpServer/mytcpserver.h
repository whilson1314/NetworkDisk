#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>   // 派生类实现客户端连接的监听和接收
#include <QList>        // 链表
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT;           // 类既要继承QObject又要写上宏Q_OBJECT，才能支持信号槽
public:
    MyTcpServer();

    static MyTcpServer &getInstance();

    void incomingConnection(qintptr socketDescriptor);

private:
    QList<MyTcpSocket*> m_tcpSocketList;        //存储服务器所有已经建立连接的Socket连接
};

#endif // MYTCPSERVER_H
