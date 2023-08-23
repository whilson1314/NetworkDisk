#include "online.h"
#include "tcpclient.h"
#include "ui_online.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    //处理pdu的MSg部分，将所有在线用户显示出来
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];

    ui -> online_listWidget -> clear();// 清除之前在线用户列表的旧数据
    for(uint i = 0; i < uiSize; i++)
    {
        memcpy(caTmp, (char *)(pdu->caMsg) + 32 * i, 32);
        qDebug() << "在线用户：" << caTmp;

//        不显示自己信息，防止之后添加自己为好友等错误操作
        if(strcmp(caTmp, TcpClient::getInstance().getStrName().toStdString().c_str()) == 0)
        {
            continue;
        }
        ui -> online_listWidget -> addItem(caTmp);
    }
}


void Online::on_addFriend_pb_clicked()
{
    QString strAddName = ui -> online_listWidget ->currentItem() -> text();//获得要添加好友的用户名
    QString StrLoginName = TcpClient::getInstance().getStrName();           //获得用户自己的用户名
    PDU *pdu = mkPDU(0);
    qDebug() << strAddName << " " << StrLoginName;
    pdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, strAddName.toStdString().c_str(), strAddName.size());
    memcpy(pdu->caData + 32, StrLoginName.toStdString().c_str(), StrLoginName.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

