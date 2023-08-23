#include "friend.h"
#include "tcpclient.h"
#include "protocol.h"
#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit;            // 显示信息
    m_pFriendListWidget = new QListWidget;   // 好友列表
    m_pInputMsgLE = new QLineEdit;           // 显示输入聊天框

    m_pDelFriendPB = new QPushButton("删除好友");        // 删除好友
    m_pFlushFriendPB = new QPushButton("刷新好友");      // 刷新好友列表
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");    // 显示/隐藏所有在线用户
    m_pSearchUsrPB = new QPushButton("查找用户");        // 查找用户
    m_pSendMsgPB = new QPushButton("发送");               // 发送按钮
    m_pPrivateChatPB = new QPushButton("私聊");        // 私聊用户


    QVBoxLayout *prightPBVBL = new QVBoxLayout; // 左侧右部分好友操作按钮布局
    prightPBVBL->addWidget(m_pPrivateChatPB);
    prightPBVBL->addWidget(m_pDelFriendPB);
    prightPBVBL->addWidget(m_pFlushFriendPB);
    prightPBVBL->addWidget(m_pShowOnlineUsrPB);
    prightPBVBL->addWidget(m_pSearchUsrPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout; // 右侧聊天布局
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(prightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;// 右侧下方发送消息布局
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pSendMsgPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain); // 将整体布局pMain设置为页面布局

    connect(m_pShowOnlineUsrPB, SIGNAL(clicked(bool)),this, SLOT(showOnline()));
    //绑定查找用户按钮与对应事件
    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)), this, SLOT(searchUser()));
    //构造函数中绑定刷新好友列表按钮与对应事件
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)),
            this, SLOT(flushFriendList()));
    //绑定删除好友按钮与对应事件
    connect(m_pDelFriendPB, SIGNAL(clicked(bool)), this, SLOT(deleteFriend()));

    // 私聊按钮
    connect(m_pPrivateChatPB, SIGNAL(clicked(bool)),
            this, SLOT(privateChat()));

    connect(m_pSendMsgPB, SIGNAL(clicked(bool)),
            this, SLOT(groupChatSendMsg()));
}

void Friend::ShowAllOnlineUser(PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }
    m_pOnline -> showUsr(pdu);
}

void Friend::setStrSearchName(const QString &strSearchName)
{
    m_strSearchName = strSearchName;
}

QString Friend::getStrSearchName() const
{
    return m_strSearchName;
}

void Friend::updateFriendList(PDU *pdu)
{
    if (NULL == pdu)
    {
        return ;
    }
    uint uiSize = pdu -> uiMsgLen / 36;         //注意是36
    char caName[32] = {'\0'};
    char caOnline[32] = {'\0'};

    m_pFriendListWidget->clear();       //清除好友列表原有数据
    for(uint i = 0; i < uiSize; ++i)
    {
        memcpy(caName, (char*)(pdu->caMsg) + i * 36, 32);
        memcpy(caOnline, (char*)(pdu->caMsg) + i * 36 + 32, 4);
        qDebug() << "客户端好友 " << caName << " " << caOnline;
            m_pFriendListWidget -> addItem(QString("%1\t%2").arg(caName)
                                         .arg(strcmp(caOnline, "1") == 0 ? "在线" : "离线"));
    }
}

QListWidget *Friend::getPFriendLW() const
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline -> isHidden())
    {
        // 显示onlineUserWid页面
        m_pOnline->show();
        // 发送请求查询数据库获取在线用户
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::searchUser()
{
    QString name = QInputDialog::getText(this, "搜索", "用户名: ");  //通过输入页面来获取用户名

    if(!name.isEmpty())
    {
        qDebug() << "查找：" << name;
        PDU *pdu = mkPDU(0);
        pdu -> uiMsgType = ENUM_MSG_TYPE_SEARCH_USERS_REQUEST;
        memcpy((char*) pdu -> caData, name.toStdString().c_str(), 32);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

//刷新好友按钮的槽函数
void Friend::flushFriendList()
{
    QString strName = TcpClient::getInstance().getStrName(); // 获取自己用户名

    PDU* pdu = mkPDU(0);
    pdu -> uiMsgType = ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST;
    strncpy(pdu -> caData, strName.toStdString().c_str(), strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

//memcpy不看是不是字符串，也不看字符串是否有’\0’，直接复制n个字节

//strncpy是复制字符串，如果碰到’\0’就停止拷贝，否则最多复制到n个字节停止拷贝
void Friend::deleteFriend()
{
    if (NULL == m_pFriendListWidget -> currentItem())   //  如果没有选中好友
    {
        return;
    }
    QString friName = m_pFriendListWidget -> currentItem() -> text();   //获得选中的好友用户名
    friName = friName.split('\t')[0];
    QString loginName = TcpClient::getInstance().getStrName();      //获得登录用户名

    qDebug() << friName;
    PDU* pdu = mkPDU(0);
    pdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    strncpy(pdu -> caData, friName.toStdString().c_str(), 32);
    strncpy(pdu -> caData + 32, loginName.toStdString().c_str(), 32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::privateChat()
{
    if (NULL == m_pFriendListWidget -> currentItem())
    {
        return ;
    }

    QString friName = m_pFriendListWidget -> currentItem() -> text();   //获得选中的好友名字
    friName = friName.split("\t")[0];
    QString loginName = TcpClient::getInstance().getStrName();      // 登录用户用户名

    privateChatWid *priChat = searchPriChatWid(friName.toStdString().c_str());

    if (priChat == NULL)
    {
        priChat = new privateChatWid;
        priChat -> setStrChatName(friName);
        priChat -> setStrLoginName(loginName);
        priChat -> setPriChatTitle(friName.toStdString().c_str());
        m_priChatWidList.append(priChat);           // 添加入该客户端私聊list
    }

    if (priChat -> isHidden())      // 如果窗口被隐藏
    {
        priChat -> show();
    }
    if (priChat -> isMinimized())   //如果窗口被最小化了
    {
        priChat -> showNormal();
    }
}

privateChatWid *Friend::searchPriChatWid(const char *chatName)
{
    for (privateChatWid *ptr:m_priChatWidList)
    {
        if (ptr -> strChatName() == chatName)
        {
            return ptr;
        }
    }
    return NULL;
}

void Friend::insertPriChatWidList(privateChatWid *priChat)
{
    m_priChatWidList.append(priChat);
}

void Friend::groupChatSendMsg()
{
    QString strMsg = m_pInputMsgLE -> text();
    if (strMsg.isEmpty())
    {
        QMessageBox::warning(this, "群聊", "发送消息不能为空！");
        return;
    }
    m_pInputMsgLE -> clear();       // 清空输入框
    m_pShowMsgTE ->append(QString("%1 : %2").arg(TcpClient::getInstance().getStrName()).arg(strMsg));

    PDU *pdu = mkPDU(strMsg.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
    strncpy(pdu -> caData, TcpClient::getInstance().getStrName().toStdString().c_str(), 32);
    strncpy((char*)(pdu -> caMsg), strMsg.toStdString().c_str(), strMsg.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::updateGroupShowMsgTE(PDU *pdu)
{
    QString strMsg = QString("%1 : %2").arg(pdu -> caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE -> append(strMsg);
}
