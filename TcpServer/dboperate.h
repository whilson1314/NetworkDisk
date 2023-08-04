#ifndef DBOPERATE_H
#define DBOPERATE_H

#include <QObject>
#include <QSqlDatabase> //连接数据库
#include <QSqlQuery>    //数据库操作

class DBoperate : public QObject
{
    Q_OBJECT
public:
    explicit DBoperate(QObject *parent = nullptr);
    static DBoperate& getInstance();    // 公用获取引用，实现单例模式
    void init();        //初始化函数，数据库连接
    ~DBoperate();       //析构函数，关闭数据库连接


    bool handleRegist(const char *name, const char *pwd);
signals:

public slots:
private:
    QSqlDatabase m_db;

};

#endif // DBOPERATE_H
