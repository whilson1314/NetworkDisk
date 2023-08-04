#include "mytcpsocket.h"
#include <QDebug>

MyTcpSocket::MyTcpSocket()
{
    //自己封装的MyTcpSocket函数，可以自己产生的socket，自己本身来接收处理
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));
}

void MyTcpSocket::recvMsg()
{
    qDebug() << this -> bytesAvailable();

}
