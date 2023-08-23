#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();  // 加载配置文件信息

    static TcpClient &getInstance();    // 将TcpClient设置为单例模式，方便外部调用socket功能
    QTcpSocket &getTcpSocket();

    QString getStrName() const;
    void setStrName(const QString &strName);

    QString getStrCurPath() const;
    void setStrCurPath(const QString &strCurPath);

    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);
public slots:
    void showConnect();
    void receMsg();

private slots:
//    void on_send_pd_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;    // 存储配置文件读取到的IP地址
    quint16 m_usPort;   // 无符号16位整型 存储配置文件的端口号

    //连接服务器，和服务进行交互
    QTcpSocket m_tcpSocket;

    QString m_strName;  // 该客户端用户名
    QString m_strRootPath;  // 用户根目录
    QString m_strCurPath;   // 当前目录
};
#endif // TCPCLIENT_H
