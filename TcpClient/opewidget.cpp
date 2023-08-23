#include "opewidget.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}
{
    /*Qt提供QListWidget类列表框控件用来加载并显示多个列表项。QListWidgetItem类就是列表项类。一般列表框控件中的列表项有两种加载方式：

一种是由用户手动添加的列表项，比如音乐播放器中加载音乐文件的文件列表，每一个音乐文件都是一个列表项。

对于这种列表项，用户可以进行增加、删除、单击以及双击等操作。

一种是由程序员事先编写好，写在程序中供用户选择的列表项，比如餐厅的电子菜单，每一道菜对应一个列表项。

对于这种列表项，用户可以进行单机和双击操作(增加和删除操作也是可以进行的，但是一般的点菜系统会屏蔽掉这种功能)。

QListWidget类列表框控件支持两种列表项显示方式，即QListView::IconMode和QListView::ListMode。

总结一下列表框常用的增加、删除、单击、双击操作以及列表项显示方式设置，先给出全部代码，再解释。*/
    m_pListW = new QListWidget(this);
    m_pListW -> addItem("好友");
    m_pListW -> addItem("文件");

    m_pFileSystem = new FileSystem;
    m_pFriend = new Friend;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pFileSystem);
    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);
    connect(m_pListW, SIGNAL(currentRowChanged(int)), m_pSW, SLOT(setCurrentIndex(int)));
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::getFriend() const
{
    return m_pFriend;
}

void OpeWidget::setUserLabel(const char *name)
{
    m_pUserLabel -> setText(name);
}

FileSystem *OpeWidget::getPFileSystem() const
{
    return m_pFileSystem;
}
