#include "mytcpserver.h"
#include <QDebug>
MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;    // 由于是静态的，所以这个函数调用多次也只是创建一次
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    // 派生QTcpSocket，然后对Socket进行绑定相应的槽函数，这样就可以不同客户端由不同MyTcpSocket进行处理
    // 从而可以实现客户端连接和对应数据收发的socket的关联
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;  // 建立新的socket连接
    pTcpSocket->setSocketDescriptor(socketDescriptor);  // 设置其Socket描述符，不同描述符指示不同客户端
    m_tcpSocketList.append(pTcpSocket);

    // 关联socket用户下线信号与删除socket的槽函数
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket *)), this, SLOT(deleteSocket(MyTcpSocket *)));
}

bool MyTcpServer::forwardMsg(const QString caDesName, PDU *pdu)
{
    if (NULL == caDesName || pdu == NULL)
    {
        return false;
    }
    // 查找目标用户名的Socket
    qDebug() << "forwardMsg()";
    for (int i = 0; i < m_tcpSocketList.size(); i++)
    {
        if (caDesName == m_tcpSocketList.at(i) -> getStrName())         //查找到
        {
            m_tcpSocketList.at(i) -> write((char*) pdu, pdu->uiPDULen); //转发消息
            return true;
        }
    }
    return false;
}

QString MyTcpServer::getStrRootPath() const
{
    return m_strRootPath;
}

void MyTcpServer::setStrRootPath(const QString &strRootPath)
{
    m_strRootPath = strRootPath;
}

void MyTcpServer::deleteSocket(MyTcpSocket *mySocket)
{
    //遍历m_tcpSocketList并删除socket
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); iter ++)
    {
        if(mySocket == *iter)
        {
            //常见错误：在释放QList中Socket空间时发生了程序错误退出
            /*
            合理的利用”delete“可以有效减少应用对内存的消耗。但是delete的不合理使用常常导致应用crash。而”deleteLater()“可以更好的规避风险, 降低崩溃。

            - 首先”deleteLater()“是QObject对象的一个函数, 要想使用此方法, 必须是一个QObject对象。
            - ”deleteLater()“依赖于Qt的event loop机制。
                - 如果在event loop启用前被调用, 那么event loop启用后对象才会被销毁;
                - 如果在event loop结束后被调用, 那么对象不会被销毁;
                - 如果在没有event loop的thread使用, 那么thread结束后销毁对象。
            - 可以多次调用此函数。
            - 线程安全。
            */
            (*iter) -> deleteLater(); // 延迟释放空间，使用delete会报错！！！
            *iter = NULL;
            m_tcpSocketList.erase(iter); // 删除列表中指针
            break;
        }
    }
    qDebug() << "--------------";
    //输出一下所有socket，看看是否删除成功 --测试

//    for (int i = 0; i < m_tcpSocketList.at(i) -> getStrName();++i)
//    {
//        QString tmp = m_tcpSocketList.at(i) -> getStrName();
//        qDebug() << tmp;
//    }
}
