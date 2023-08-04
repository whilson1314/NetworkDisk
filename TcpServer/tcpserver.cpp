#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>


TcpServer::TcpServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
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
        //        qDebug() << m_strIP;
        //        qDebug() << m_usPort;
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

