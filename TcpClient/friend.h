#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"
#include "protocol.h"
#include "privatechatwid.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);

    void ShowAllOnlineUser(PDU *pdu);    // 显示所有在线用户的信息
    void setStrSearchName(const QString &strSearchName);
    QString getStrSearchName() const;       //得到查询的用户名

    void updateFriendList(PDU *pdu);        //刷新好友列表
    QListWidget *getPFriendLW() const;
signals:

public slots:
    void showOnline();  //显示在线用户
    void searchUser();  //查找用户
    void flushFriendList(); //刷新用户列表
    void deleteFriend();//删除好友
    void privateChat();               // 私聊按钮的槽函数
    privateChatWid* searchPriChatWid(const char* chatName); // 获得对应用户名的私聊窗口
    void insertPriChatWidList(privateChatWid* priChat); // 将私聊窗口插入到私聊窗口List
    void groupChatSendMsg();          // 群聊发送按钮的槽函数
    void updateGroupShowMsgTE(PDU* pdu); // 更新群聊showMsgTE聊天消息窗口的消息


private:
    QTextEdit *m_pShowMsgTE;            // 显示信息
    QListWidget *m_pFriendListWidget;   // 好友列表
    QLineEdit *m_pInputMsgLE;           // 显示输入聊天框

    QPushButton *m_pDelFriendPB;        // 删除好友
    QPushButton *m_pFlushFriendPB;      // 刷新好友列表
    QPushButton *m_pShowOnlineUsrPB;    // 显示/隐藏所有在线用户
    QPushButton *m_pSearchUsrPB;        // 查找用户
    QPushButton *m_pPrivateChatPB;      // 私聊按钮
    QPushButton *m_pSendMsgPB;          // 发送按钮

    QString m_strSearchName;          // 查找的用户的名字
    Online *m_pOnline;

    QList<privateChatWid*> m_priChatWidList; // 所有私聊的窗口
};

#endif // FRIEND_H
