#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>
#include <QDateTime>

class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    static DatabaseHandler* getInstance();
    ~DatabaseHandler();

    bool connectToDatabase(const QString& hostName, const QString& databaseName,
                          const QString& username, const QString& password,
                          quint16 port = 3306);
    
    void disconnectFromDatabase();
    bool isConnected() const;

    // 用户相关操作
    QJsonObject validateUser(const QString& username, const QString& password);
    QJsonObject getUserInfo(int userId);
    bool updateUserInfo(const QJsonObject& userInfo);
    bool addUser(const QJsonObject& userInfo);
    bool deleteUser(int userId);
    QJsonArray getUsers(int userType = -1);

    // 课题相关操作
    bool addTopic(const QJsonObject& topicInfo);
    bool updateTopic(const QJsonObject& topicInfo);
    bool deleteTopic(int topicId);
    QJsonObject getTopic(int topicId);
    QJsonArray getTopics(int status = -1, int teacherId = -1);

    // 选题申请相关操作
    bool addApplication(const QJsonObject& applicationInfo);
    bool updateApplicationStatus(int applicationId, int status, const QString& rejectReason = "", int reviewerId = 0);
    QJsonObject getApplication(int applicationId);
    QJsonArray getApplications(int groupId = -1, int topicId = -1, int status = -1);

    // 教师相关操作
    QJsonObject getTeacherInfo(int teacherId);
    bool updateTeacherInfo(const QJsonObject& teacherInfo);

    // 学生小组相关操作
    QJsonObject getStudentGroupInfo(int groupId);
    bool updateStudentGroupInfo(const QJsonObject& groupInfo);

    // 统计信息
    QJsonObject getStatistics();

private:
    explicit DatabaseHandler(QObject *parent = nullptr);
    static DatabaseHandler* instance;
    mutable QMutex mutex;

    QSqlDatabase db;
    QString lastError;

    bool executeQuery(const QString& queryStr, const QVariantList& params = QVariantList());
    QJsonObject queryToJsonObject(QSqlQuery& query);
    QJsonArray queryToJsonArray(QSqlQuery& query);
};

#endif // DATABASEHANDLER_H