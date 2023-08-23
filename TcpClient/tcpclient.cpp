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
    QFile file(":/client.config");  // 文件对象，读取资源文件 ':' + "前缀" + "文件名"
    if(file.open(QIODevice::ReadOnly))// file.open() 参数：打开方式：只读（注意，这里只读是写在QIODevice下的枚举，所以调用要声明命名空间） 返回true则打开成功
    {
        QByteArray baData = file.readAll(); // 读出所有数据，返回字节数组QByteArray
        QString strData = baData.toStdString().c_str();// 转换为字符串 注意std::string不能自动转为QString，还需转为char*
        file.close();
        strData.replace("\r\n", " ");   // 替换IP地址与端口号之间\r\n
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort =  strList.at(1).toUShort();   // 无符号短整型
        qDebug() << "ip : " << m_strIP << " port : " << m_usPort;   // 打印结果
    }
    else    // 文件打开失败则弹出提示窗口
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::getStrName() const
{
    return m_strName;
}

QString TcpClient::getStrRootPath() const
{
    return m_strRootPath;
}

void TcpClient::setStrRootPath(const QString &strRootPath)
{
    m_strRootPath = strRootPath;
}


QString TcpClient::getStrCurPath() const
{
    return m_strCurPath;
}

void TcpClient::setStrCurPath(const QString &strCurPath)
{
    m_strCurPath = strCurPath;
}


void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
    //    qDebug() << "连接服务器成功";
}

void TcpClient::receMsg()
{
    // 如果处于接受文件数据的状态
    TransFile *transFile = OpeWidget::getInstance().getPFileSystem()->getDownloadFileInfo();
    if(transFile->bTransform)
    {
        QByteArray baBuffer = m_tcpSocket.readAll();
        transFile->file.write(baBuffer);

        transFile->iReceivedSize += baBuffer.size();
        if(transFile->iReceivedSize == transFile->iTotalSize)
        {

            QMessageBox::information(this, "下载文件", "下载文件成功！");
            transFile->file.close();
            transFile->file.setFileName("");
            transFile->bTransform = false;
            transFile->iTotalSize = 0;
            transFile->iReceivedSize = 0;
        }
        else if(transFile->iReceivedSize > transFile->iTotalSize)
        {
            QMessageBox::warning(this, "下载文件", "下载文件失败！");
            transFile->file.close();
            transFile->file.setFileName("");
            transFile->bTransform = false;
            transFile->iTotalSize = 0;
            transFile->iReceivedSize = 0;
        }
        return ;
    }


//    qDebug() << m_tcpSocket.bytesAvailable(); // 输出接收到的数据大小
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint)); // 先读取uint大小的数据，首个uint正是总数据大小
    uint uiMsgLen = uiPDULen - sizeof(PDU); // 实际消息大小，sizeof(PDU)只会计算结构体大小，而不是分配的大小
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收剩余部分数据（第一个uint已读取）
//    qDebug() << pdu -> uiMsgType << ' ' << (char*)pdu -> caMsg; // 输出

    // 根据不同消息类型，执行不同操作
    switch(pdu -> uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND: // 注册响应
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
    case ENUM_MSG_TYPE_LOGIN_RESPOND: // 登录响应
    {
//        qDebug() << "login" ;
        if(0 == strcmp(pdu -> caData, LOGIN_OK))
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu -> caData + 32, 32); // 设置已登录用户名
            //设置用户根目录和当前目录
            m_strRootPath = QString((char*)pdu->caMsg);
            qDebug() << "用户根目录 " << m_strRootPath;
            m_strCurPath = m_strRootPath;
            m_strName = caName;
            qDebug() << "用户已登录：" << caName << " strName：" << m_strName;
//            QMessageBox::information(this, "登录", LOGIN_OK);
//            OpeWidget::getInstance().setUserLabel(caName);          // 设置主页面用户信息
            OpeWidget::getInstance().show();
            // 默认请求一次好友列表
            OpeWidget::getInstance().getFriend() -> flushFriendList();
//            OpeWidget::getInstance().getPFileSystem() -> flushDir();
            this->hide();
        }
        else if(0 == strcmp(pdu -> caData, LOGIN_FAILED))
        {
            QMessageBox::warning(this, "登录", LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USERS_RESPOND:    //查询所有在线用户响应
    {
//        qDebug() << "========";
        OpeWidget::getInstance().getFriend() -> ShowAllOnlineUser(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USERS_RESPOND:    //查找用户响应
    {
//        qDebug() << pdu->caData << "++++++++++";
        if(0 == strcmp(SEARCH_USER_OK, pdu->caData))
        {
            QMessageBox::information(this, "查找", OpeWidget::getInstance().getFriend()->getStrSearchName() + SEARCH_USER_OK);
        }
        else if (0 == strcmp(SEARCH_USER_OFFLINE, pdu->caData))
        {
            QMessageBox::information(this, "查找", OpeWidget::getInstance().getFriend()->getStrSearchName() + SEARCH_USER_OFFLINE);
        }
        else if (0 == strcmp(SEARCH_USER_EMPTY, pdu->caData))
        {
            QMessageBox::warning(this, "查找", OpeWidget::getInstance().getFriend()->getStrSearchName() + SEARCH_USER_EMPTY);
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND: // 好友请求回复消息
    {
        QMessageBox::information(this, "添加好友", pdu -> caData);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:       // 处理服务器转发过来的好友请求消息
    {
        char sourceName[32];        //获取发送方用户名
        strncpy(sourceName, pdu->caData + 32, 32);
        int ret = QMessageBox::information(this, "好友申请", QString("%1 想添加您为好友，是否同意？").arg(sourceName), QMessageBox::Yes, QMessageBox::No);   // 后面两个参数是为QMessage默认支持两个按钮来设置枚举值
        PDU* resPdu = mkPDU(0);

        strncpy(resPdu -> caData, pdu->caData, 32); //被加好友者名字
        strncpy(resPdu -> caData + 32, pdu -> caData + 32, 32); //加好友者用户名
        qDebug() << "同意加好友吗？" << resPdu -> caData << " " << resPdu -> caData + 32;
        if(ret == QMessageBox::Yes)
        {
            resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }
        else
        {
            resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
        }
        m_tcpSocket.write((char*)resPdu, resPdu -> uiPDULen);       //发送给服务器消息，由服务器写入数据库并转发给用户
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:        //对方同意添加好友
    {
        QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请！").arg(pdu ->caData));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REJECT:       //对方拒绝添加好友
    {
        QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(pdu -> caData));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:    // 刷新好友响应
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:   //删除好友相应
    {
        QMessageBox::information(this, "删除好友", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:   // 处理服务器转发过来的删除好友请求
    {
        char sourceName[32];   // 获得发送方用户名
        strncpy(sourceName, pdu -> caData + 32, 32);
        QMessageBox::information(this, "删除好友", QString("%1 已删除与您的好友关系！").arg(sourceName));
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND: // 私聊好友消息响应（发送消息是否成功）
    {
        if(strcmp(PRIVATE_CHAT_OFFLINE, pdu -> caData) == 0) // 发送消息失败
        {
            QMessageBox::information(this, "私聊", PRIVATE_CHAT_OFFLINE);
        }
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:        // 私聊好友消息请求（接受消息）
    {
        char sourceName[32];        //获取发送方用户名
        strncpy(sourceName, pdu -> caData + 32, 32);
        privateChatWid *priChatW = OpeWidget::getInstance().getFriend() -> searchPriChatWid(sourceName);
        if (NULL == priChatW)
        {
            priChatW = new privateChatWid;
            priChatW -> setStrChatName(sourceName);
            priChatW -> setStrLoginName(m_strName);
            priChatW -> setPriChatTitle(sourceName);
            OpeWidget::getInstance().getFriend()->insertPriChatWidList(priChatW);
        }
        priChatW -> updateShowMsgTE(pdu);
        priChatW -> show();
        if ( priChatW -> isMinimized()) // 如果窗口被最小化了
        {
            priChatW -> showNormal();
        }
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:      // 群发好友信息请求（接收消息）
    {
        OpeWidget::getInstance().getFriend() -> updateGroupShowMsgTE(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:      //创建文件夹相应
    {
        QMessageBox::information(this, "创建文件夹", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_DIR_RESPOND:       // 刷新文件夹响应
    {
        OpeWidget::getInstance().getPFileSystem()->updateFileList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_RESPOND:     // 删除文件或文件夹响应
    {
        QMessageBox::information(this, "删除文件", pdu -> caData);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND: // 重命名文件或文件夹响应
    {
        QMessageBox::information(this, "重命名文件", pdu -> caData);
        break;
    }
    case ENUM_MSG_TYPE_ENTRY_DIR_RESPOND: // 进入文件夹响应
    {
        qDebug() << "进入文件夹响应：" << pdu -> caData;
        if(strcmp(ENTRY_DIR_OK, pdu -> caData) == 0)
        {
            OpeWidget::getInstance().getPFileSystem() -> updateFileList(pdu); // 刷新文件列表
            QString entryPath = OpeWidget::getInstance().getPFileSystem()->strTryEntryDir();
            if(!entryPath.isEmpty())
            {
                m_strCurPath = entryPath;
                OpeWidget::getInstance().getPFileSystem()->clearStrTryEntryDir(); // 清空m_strTryEntryDir
                qDebug() << "当前路径：" << m_strCurPath;
            }
        }
        else
        {
            QMessageBox::warning(this, "进入文件夹", pdu -> caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_PRE_DIR_RESPOND: // 上一目录响应
    {
        qDebug() << "上一文件夹响应：" << pdu -> caData;
        if(strcmp(PRE_DIR_OK, pdu -> caData) == 0)
        {
            OpeWidget::getInstance().getPFileSystem() -> updateFileList(pdu); // 刷新文件列表
            QString entryPath = OpeWidget::getInstance().getPFileSystem()->strTryEntryDir();
            if(!entryPath.isEmpty())
            {
                m_strCurPath = entryPath;
                OpeWidget::getInstance().getPFileSystem()->clearStrTryEntryDir(); // 清空m_strTryEntryDir
                qDebug() << "当前路径：" << m_strCurPath;
            }
        }
        else
        {
            QMessageBox::warning(this, "上一文件夹", pdu -> caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND: // 上传文件响应
    {
        if(strcmp(UPLOAD_FILE_START, pdu -> caData) == 0) // 开始上传文件数据内容
        {
            OpeWidget::getInstance().getPFileSystem()->startTimer();
        }
        else if(strcmp(UPLOAD_FILE_OK, pdu -> caData) == 0) // 上传文件成功
        {
            QMessageBox::information(this, "上传文件", pdu -> caData);
        }
        else if(strcmp(UPLOAD_FILE_FAILED, pdu -> caData) == 0) // 上传失败
        {
            QMessageBox::warning(this, "上传文件", pdu -> caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND: // 下载文件响应
    {
        if(strcmp(DOWNLOAD_FILE_START, pdu -> caData) == 0) // 开始下载文件数据内容
        {
            // TransFile *transFile = OperateWidget::getInstance().getPFileSystem()->getDownloadFileInfo();
            qint64 ifileSize = 0;
            char strFileName[32];
            sscanf((char*)pdu -> caMsg, "%s %lld", strFileName, &ifileSize);
            qDebug() << "下载文件中：" << strFileName << ifileSize;

            if(strlen(strFileName) > 0 && transFile->file.open(QIODevice::WriteOnly))
            {
                transFile->bTransform = true;
                transFile->iTotalSize = ifileSize;
                transFile->iReceivedSize = 0;
            }
            else
            {
                QMessageBox::warning(this, "下载文件", "下载文件失败！");
            }
        }
        else if(strcmp(DOWNLOAD_FILE_OK, pdu -> caData) == 0) // 下载文件成功
        {
            QMessageBox::information(this, "下载文件", pdu -> caData);
        }
        else if(strcmp(DOWNLOAD_FILE_FAILED, pdu -> caData) == 0) // 下载失败
        {
            QMessageBox::warning(this, "下载文件", pdu -> caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND: // 移动文件响应
    {
        QMessageBox::information(this, "移动文件", pdu -> caData);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND: // 分享文件响应
    {
        QMessageBox::information(this, "分享文件", pdu -> caData);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE: // 被分享文件提醒
    {
        char caFileName[32]; // 文件名
        char caSouName[32]; // 用户名
        int iFilePathLen = pdu -> uiMsgLen;
        char caFilePath[iFilePathLen]; // 文件路径

        memcpy(caSouName, pdu -> caData, 32);
        memcpy(caFileName, pdu -> caData + 32, 32);
        QString strShareNote = QString("%1 想要分享 %2 文件给您，\n是否接收？").arg(caSouName).arg(caFileName);
        QMessageBox::StandardButton sbShareNote = QMessageBox::question(this, "分享文件", strShareNote);
        if(sbShareNote == QMessageBox::No)
        { // 拒绝接收
            break;
        }

        // 同意接收
        qDebug() << "接收文件：" << caSouName <<" " << caFileName;
        memcpy(caFilePath, (char*)pdu -> caMsg, iFilePathLen);
        QString strRootDir = m_strRootPath; // 用户根目录
        PDU *resPdu = mkPDU(iFilePathLen + strRootDir.size() + 1);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
        sprintf(resPdu -> caData, "%d %d", iFilePathLen, strRootDir.size());
        sprintf((char*)resPdu -> caMsg, "%s %s", caFilePath, strRootDir.toStdString().c_str());
        qDebug() << (char*)resPdu -> caMsg;
        m_tcpSocket.write((char*)resPdu, resPdu -> uiPDULen);
        free(resPdu);
        resPdu = NULL;

        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND: // 被分享文件通知响应的处理结果
    {
        QMessageBox::information(this, "分享文件", pdu -> caData);
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
    QString strName = ui->name_le->text();  //获取用户名和密码
    QString strPwd = ui->pwd_le->text();
    //合理性判断
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        m_strName = strName;
        //注册信息用户名和密码将用caData[64]传输
        PDU *pdu = mkPDU(0);    //实际消息体积为0
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;  //设置为注册请求消息类型
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
        QMessageBox::critical(this, "登录", "登录失败:用户名或者密码为空");
    }

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

