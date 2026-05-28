#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>

class DatabaseConnection
{
public:
    // 获取单例实例
    static DatabaseConnection& getInstance()
    {
        static DatabaseConnection instance;
        return instance;
    }

    // 禁止拷贝构造和赋值操作
    DatabaseConnection(const DatabaseConnection&) = delete;
    void operator=(const DatabaseConnection&) = delete;

    // 连接数据库
    bool connect(const QString& hostName = "localhost",
                 const QString& databaseName = "student_topic_system",
                 const QString& userName = "root",
                 const QString& password = "123456",
                 int port = 3306);

    // 断开数据库连接
    void disconnect();

    // 检查数据库连接状态
    bool isConnected() const;

    // 获取数据库实例
    QSqlDatabase getDatabase();

    // 执行SQL查询
    QSqlQuery executeQuery(const QString& sql);

    // 执行SQL命令（插入、更新、删除）
    bool executeCommand(const QString& sql);

    // 事务操作
    bool startTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

private:
    // 私有构造函数
    DatabaseConnection();
    ~DatabaseConnection();

    QSqlDatabase db;
    QString connectionName;
};

#endif // DATABASECONNECTION_H