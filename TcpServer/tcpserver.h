#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include "mytcpserver.h" // QTcpServer派生类实现监听和接收客户端的TCP连接

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QMainWindow
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();

private:
    Ui::TcpServer *ui;

    QString m_strIP;
    quint16 m_usPort;
};
#endif // TCPSERVER_H
