#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

//数据库密码 123mysql
TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(500, 300);
    //读取配置文件
    loadConfig();

    // 绑定连接服务器信号处理的槽函数
    connect(&m_tcpSocket, SIGNAL(connected()), // 信号发送方（Socket变量），发送信号类型
            this, SLOT(showConnect())); // 信号处理方，用以处理的槽函数
    // 绑定处理服务器响应消息的槽函数
    connect(&m_tcpSocket, SIGNAL(readyRead()), // 信号发送方（Socket变量），发送信号类型
            this, SLOT(receMsg())); // 信号处理方，用以处理的槽函数

    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort =  strList.at(1).toUShort();
        qDebug() << "ip : " << m_strIP << " port : " << m_usPort;
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
    //    qDebug() << "连接服务器成功";
}

void TcpClient::receMsg()
{
    qDebug() << m_tcpSocket.bytesAvailable(); // 输出接收到的数据大小
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint)); // 先读取uint大小的数据，首个uint正是总数据大小
    uint uiMsgLen = uiPDULen - sizeof(PDU); // 实际消息大小，sizeof(PDU)只会计算结构体大小，而不是分配的大小
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收剩余部分数据（第一个uint已读取）
    qDebug() << pdu -> uiMsgType << ' ' << (char*)pdu -> caMsg; // 输出

    // 根据不同消息类型，执行不同操作
    switch(pdu -> uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND: // 注册请求
    {
        if(0 == strcmp(pdu -> caData, REGIST_OK))
        {
            QMessageBox::information(this, "注册", REGIST_OK);
        }
        else if(0 == strcmp(pdu -> caData, REGIST_FAILED))
        {
            QMessageBox::warning(this, "注册", REGIST_FAILED);
        }
        break;
    }
    default:
        break;
    }

    // 释放空间
    free(pdu);
    pdu = NULL;
}

#if 0
void TcpClient::on_send_pd_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.size());
        pdu -> uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size());
        m_tcpSocket.write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "信息发送", "发送信息不能为空");
    }
}
#endif

void TcpClient::on_login_pb_clicked()
{

}


void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();  //获取用户名和密码
    QString strPwd = ui->pwd_le->text();

    //合理性判断
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        //注册信息用户名和密码将用caData[64]传输
        PDU *pdu = mkPDU(0);    //实际消息体积为0
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;  //设置为注册请求消息类型
        //拷贝用户名和密码信息到caData
        memcpy(pdu->caData, strName.toStdString().c_str(), 32); //由于数据库设定的32位，所以最多只拷贝前32位
        memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);   //发送消息

        //释放空间
        free(pdu);
        pdu = NULL;

    }
    else
    {
        QMessageBox::critical(this, "注册", "注册失败:用户名或者密码为空");
    }


}


void TcpClient::on_cancel_pb_clicked()
{

}

