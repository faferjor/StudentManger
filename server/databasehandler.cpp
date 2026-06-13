#include "databasehandler.h"
#include <QDebug>
#include <QJsonDocument>

DatabaseHandler* DatabaseHandler::instance = nullptr;

DatabaseHandler::DatabaseHandler(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
}

DatabaseHandler::~DatabaseHandler()
{
    disconnectFromDatabase();
}

DatabaseHandler* DatabaseHandler::getInstance()
{
    static QMutex mutex;
    if (!instance) {
        QMutexLocker locker(&mutex);
        if (!instance) {
            instance = new DatabaseHandler();
        }
    }
    return instance;
}

bool DatabaseHandler::connectToDatabase(const QString& hostName, const QString& databaseName,
                                       const QString& username, const QString& password,
                                       quint16 port)
{
    QMutexLocker locker(&mutex);

    if (db.isOpen()) {
        db.close();
    }

    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(username);
    db.setPassword(password);
    db.setPort(port);

    bool success = db.open();
    if (!success) {
        lastError = db.lastError().text();
        qCritical() << "Database connection failed:" << lastError;
    } else {
        qInfo() << "Database connected successfully";
    }

    return success;
}

void DatabaseHandler::disconnectFromDatabase()
{
    QMutexLocker locker(&mutex);
    if (db.isOpen()) {
        db.close();
        qInfo() << "Database disconnected";
    }
}

bool DatabaseHandler::isConnected() const
{
    QMutexLocker locker(const_cast<QMutex*>(&mutex));
    return db.isOpen();
}

QJsonObject DatabaseHandler::validateUser(const QString& username, const QString& password)
{

    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    QString queryStr = "SELECT user_id, username, user_type, real_name, status "
                      "FROM users WHERE username = ? AND password = MD5(?)";

    query.prepare(queryStr);
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Validate user failed:" << lastError;
        return QJsonObject();
    }

   // qDebug() << "validateUser query:" << queryStr;

    if (query.next()) {
        int status = query.value(4).toInt();
        if (status != 1) {
            lastError = "User account is disabled";
            return QJsonObject();
        }

        QJsonObject userInfo;
        userInfo["user_id"] = query.value(0).toInt();
        userInfo["username"] = query.value(1).toString();
        userInfo["user_type"] = query.value(2).toInt();
        userInfo["real_name"] = query.value(3).toString();
        userInfo["status"] = status;

        return userInfo;
    }

    lastError = "Invalid username or password";
    return QJsonObject();

    //qDebug() << "validateUser query:" << queryStr;
}

QJsonObject DatabaseHandler::getUserInfo(int userId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    QString queryStr = "SELECT user_id, username, user_type, real_name, email, phone, status, create_time "
                      "FROM users WHERE user_id = ?";

    query.prepare(queryStr);
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get user info failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        QJsonObject userInfo;
        userInfo["user_id"] = query.value(0).toInt();
        userInfo["username"] = query.value(1).toString();
        userInfo["user_type"] = query.value(2).toInt();
        userInfo["real_name"] = query.value(3).toString();
        userInfo["email"] = query.value(4).toString();
        userInfo["phone"] = query.value(5).toString();
        userInfo["status"] = query.value(6).toInt();
        userInfo["create_time"] = query.value(7).toDateTime().toString(Qt::ISODate);

        // 获取用户详细信息
        int userType = query.value(2).toInt();
        if (userType == 2) { // 教师
            queryStr = "SELECT department, title, research_area, teacher_id FROM teachers WHERE user_id = ?";
            query.prepare(queryStr);
            query.addBindValue(userId);
            if (query.exec() && query.next()) {
                userInfo["department"] = query.value(0).toString();
                userInfo["title"] = query.value(1).toString();
                userInfo["research_area"] = query.value(2).toString();
                userInfo["teacher_id"] = query.value(3).toInt();
            }
        } else if (userType == 3) { // 学生小组
            queryStr = "SELECT leader_name, member_count, members, major, grade, class_name, group_id "
                       "FROM student_groups WHERE user_id = ?";
            query.prepare(queryStr);
            query.addBindValue(userId);
            if (query.exec() && query.next()) {
                userInfo["leader_name"] = query.value(0).toString();
                userInfo["member_count"] = query.value(1).toInt();
                userInfo["members"] = query.value(2).toString();
                userInfo["major"] = query.value(3).toString();
                userInfo["grade"] = query.value(4).toString();
                userInfo["class_name"] = query.value(5).toString();
                userInfo["group_id"] = query.value(6).toInt();
            }
        }

        return userInfo;
    }

    lastError = "User not found";
    return QJsonObject();
}

QJsonArray DatabaseHandler::getUsers(int userType)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonArray();
    }

    QSqlQuery query;
    QString queryStr = "SELECT user_id, username, user_type, real_name, email, phone, status, create_time "
                      "FROM users";

    if (userType != -1) {
        queryStr += " WHERE user_type = ?";
        query.prepare(queryStr);
        query.addBindValue(userType);
    } else {
        query.prepare(queryStr);
    }

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get users failed:" << lastError;
        return QJsonArray();
    }

    return queryToJsonArray(query);
}

bool DatabaseHandler::addUser(const QJsonObject& userInfo)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    db.transaction();

    QSqlQuery query;
    QString queryStr = "INSERT INTO users (username, password, user_type, real_name, email, phone, status) "
                      "VALUES (?, MD5(?), ?, ?, ?, ?, ?)";

    query.prepare(queryStr);
    query.addBindValue(userInfo["username"].toString());
    query.addBindValue(userInfo["password"].toString());
    query.addBindValue(userInfo["user_type"].toInt());
    query.addBindValue(userInfo["real_name"].toString());
    query.addBindValue(userInfo["email"].toString());
    query.addBindValue(userInfo["phone"].toString());
    query.addBindValue(userInfo["status"].toInt(1));

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Add user failed:" << lastError;
        db.rollback();
        return false;
    }

    int userId = query.lastInsertId().toInt();
    int userType = userInfo["user_type"].toInt();

    if (userType == 2) { // 教师
        queryStr = "INSERT INTO teachers (user_id, department, title, research_area) "
                  "VALUES (?, ?, ?, ?)";
        query.prepare(queryStr);
        query.addBindValue(userId);
        query.addBindValue(userInfo["department"].toString());
        query.addBindValue(userInfo["title"].toString());
        query.addBindValue(userInfo["research_area"].toString());

        if (!query.exec()) {
            lastError = query.lastError().text();
            qCritical() << "Add teacher info failed:" << lastError;
            db.rollback();
            return false;
        }
    } else if (userType == 3) { // 学生小组
        queryStr = "INSERT INTO student_groups (user_id, leader_name, member_count, members, major, grade, class_name) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)";
        query.prepare(queryStr);
        query.addBindValue(userId);
        query.addBindValue(userInfo["leader_name"].toString());
        query.addBindValue(userInfo["member_count"].toInt(1));
        query.addBindValue(userInfo["members"].toString());
        query.addBindValue(userInfo["major"].toString());
        query.addBindValue(userInfo["grade"].toString());
        query.addBindValue(userInfo["class_name"].toString());

        if (!query.exec()) {
            lastError = query.lastError().text();
            qCritical() << "Add student group info failed:" << lastError;
            db.rollback();
            return false;
        }
    }

    db.commit();
    return true;
}

bool DatabaseHandler::updateUserInfo(const QJsonObject& userInfo)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    db.transaction();

    int userId = userInfo["user_id"].toInt();
    QSqlQuery query;
    QString queryStr = "UPDATE users SET real_name = ?, email = ?, phone = ?, status = ? "
                      "WHERE user_id = ?";

    query.prepare(queryStr);
    query.addBindValue(userInfo["real_name"].toString());
    query.addBindValue(userInfo["email"].toString());
    query.addBindValue(userInfo["phone"].toString());
    query.addBindValue(userInfo["status"].toInt());
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Update user info failed:" << lastError;
        db.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        lastError = "User not found";
        db.rollback();
        return false;
    }

    int userType = userInfo["user_type"].toInt();
    if (userType == 2) { // 教师
        queryStr = "UPDATE teachers SET department = ?, title = ?, research_area = ? "
                  "WHERE user_id = ?";
        query.prepare(queryStr);
        query.addBindValue(userInfo["department"].toString());
        query.addBindValue(userInfo["title"].toString());
        query.addBindValue(userInfo["research_area"].toString());
        query.addBindValue(userId);

        if (!query.exec()) {
            lastError = query.lastError().text();
            qCritical() << "Update teacher info failed:" << lastError;
            db.rollback();
            return false;
        }
    } else if (userType == 3) { // 学生小组
        queryStr = "UPDATE student_groups SET leader_name = ?, member_count = ?, members = ?, "
                  "major = ?, grade = ?, class_name = ? WHERE user_id = ?";
        query.prepare(queryStr);
        query.addBindValue(userInfo["leader_name"].toString());
        query.addBindValue(userInfo["member_count"].toInt());
        query.addBindValue(userInfo["members"].toString());
        query.addBindValue(userInfo["major"].toString());
        query.addBindValue(userInfo["grade"].toString());
        query.addBindValue(userInfo["class_name"].toString());
        query.addBindValue(userId);

        if (!query.exec()) {
            lastError = query.lastError().text();
            qCritical() << "Update student group info failed:" << lastError;
            db.rollback();
            return false;
        }
    }

    db.commit();
    return true;
}

bool DatabaseHandler::deleteUser(int userId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    db.transaction();

    // 首先获取用户类型，确定需要删除哪些相关数据
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT user_type FROM users WHERE user_id = ?");
    checkQuery.addBindValue(userId);
    if (!checkQuery.exec()) {
        lastError = checkQuery.lastError().text();
        qCritical() << "Check user type failed:" << lastError;
        db.rollback();
        return false;
    }

    int userType = 0;
    if (checkQuery.next()) {
        userType = checkQuery.value(0).toInt();
    } else {
        lastError = "User not found";
        db.rollback();
        return false;
    }

    // 先删除相关联的表记录（外键约束）
    if (userType == 2) {
        // 教师 - 删除 teachers 表中的记录
        QSqlQuery deleteTeacherQuery;
        deleteTeacherQuery.prepare("DELETE FROM teachers WHERE user_id = ?");
        deleteTeacherQuery.addBindValue(userId);
        if (!deleteTeacherQuery.exec()) {
            lastError = deleteTeacherQuery.lastError().text();
            qCritical() << "Delete teacher record failed:" << lastError;
            db.rollback();
            return false;
        }
    } else if (userType == 3) {
        // 学生小组 - 删除 student_groups 表中的记录
        QSqlQuery deleteGroupQuery;
        deleteGroupQuery.prepare("DELETE FROM student_groups WHERE user_id = ?");
        deleteGroupQuery.addBindValue(userId);
        if (!deleteGroupQuery.exec()) {
            lastError = deleteGroupQuery.lastError().text();
            qCritical() << "Delete student group record failed:" << lastError;
            db.rollback();
            return false;
        }
    }

    // 最后删除 users 表中的记录
    QSqlQuery query;
    QString queryStr = "DELETE FROM users WHERE user_id = ?";
    query.prepare(queryStr);
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Delete user failed:" << lastError;
        db.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        lastError = "User not found";
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

QJsonObject DatabaseHandler::queryToJsonObject(QSqlQuery& query)
{
    QJsonObject obj;
    QSqlRecord record = query.record();

    for (int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        QVariant value = query.value(i);

        switch (value.typeId()) {
        case QMetaType::Int:
            obj[fieldName] = value.toInt();
            break;
        case QMetaType::Double:
            obj[fieldName] = value.toDouble();
            break;
        case QMetaType::QString:
            obj[fieldName] = value.toString();
            break;
        case QMetaType::QDateTime:
            obj[fieldName] = value.toDateTime().toString(Qt::ISODate);
            break;
        default:
            obj[fieldName] = value.toString();
            break;
        }
    }

    return obj;
}

QJsonArray DatabaseHandler::queryToJsonArray(QSqlQuery& query)
{
    QJsonArray array;

    while (query.next()) {
        array.append(queryToJsonObject(query));
    }

    return array;
}

// 课题相关操作实现
bool DatabaseHandler::addTopic(const QJsonObject& topicInfo)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    QSqlQuery query;
    QString queryStr = "INSERT INTO topics (topic_name, teacher_id, description, requirements, max_students, status) "
                      "VALUES (?, ?, ?, ?, ?, ?)";

    query.prepare(queryStr);
    query.addBindValue(topicInfo["topic_name"].toString());
    query.addBindValue(topicInfo["teacher_id"].toInt());
    query.addBindValue(topicInfo["description"].toString());
    query.addBindValue(topicInfo["requirements"].toString());
    query.addBindValue(topicInfo["max_students"].toInt(5));
    query.addBindValue(topicInfo["status"].toInt(1));

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Add topic failed:" << lastError;
        return false;
    }

    return true;
}

bool DatabaseHandler::updateTopic(const QJsonObject& topicInfo)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    QSqlQuery query;
    QString queryStr = "UPDATE topics SET topic_name = ?, teacher_id = ?, description = ?, "
                      "requirements = ?, max_students = ?, status = ? WHERE topic_id = ?";

    query.prepare(queryStr);
    query.addBindValue(topicInfo["topic_name"].toString());
    query.addBindValue(topicInfo["teacher_id"].toInt());
    query.addBindValue(topicInfo["description"].toString());
    query.addBindValue(topicInfo["requirements"].toString());
    query.addBindValue(topicInfo["max_students"].toInt());
    query.addBindValue(topicInfo["status"].toInt());
    query.addBindValue(topicInfo["topic_id"].toInt());

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Update topic failed:" << lastError;
        return false;
    }

    if (query.numRowsAffected() == 0) {
        lastError = "Topic not found";
        return false;
    }

    return true;
}

bool DatabaseHandler::deleteTopic(int topicId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    QSqlQuery query;
    QString queryStr = "DELETE FROM topics WHERE topic_id = ?";

    query.prepare(queryStr);
    query.addBindValue(topicId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Delete topic failed:" << lastError;
        return false;
    }

    if (query.numRowsAffected() == 0) {
        lastError = "Topic not found";
        return false;
    }

    return true;
}

QJsonObject DatabaseHandler::getTopic(int topicId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    QString queryStr = "SELECT * FROM topics WHERE topic_id = ?";

    query.prepare(queryStr);
    query.addBindValue(topicId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get topic failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        return queryToJsonObject(query);
    }

    lastError = "Topic not found";
    return QJsonObject();
}

QJsonArray DatabaseHandler::getTopics(int status, int teacherId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonArray();
    }

    QSqlQuery query;
    QString queryStr = "SELECT * FROM topics";
    QStringList conditions;
    QVariantList params;

    if (status != -1) {
        conditions << "status = ?";
        params << status;
    }

    if (teacherId != -1) {
        conditions << "teacher_id = ?";
        params << teacherId;
    }

    if (!conditions.isEmpty()) {
        queryStr += " WHERE " + conditions.join(" AND ");
    }

    query.prepare(queryStr);
    for (const QVariant& param : params) {
        query.addBindValue(param);
    }

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get topics failed:" << lastError;
        return QJsonArray();
    }

    return queryToJsonArray(query);
}

// 选题申请相关操作实现
bool DatabaseHandler::addApplication(const QJsonObject& applicationInfo)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    // 检查是否已经申请过该课题
    QSqlQuery query;
    QString queryStr = "SELECT application_id FROM topic_applications WHERE group_id = ? AND topic_id = ?";
    query.prepare(queryStr);
    query.addBindValue(applicationInfo["group_id"].toInt());
    query.addBindValue(applicationInfo["topic_id"].toInt());
    
    if (query.exec() && query.next()) {
        lastError = "Group has already applied for this topic";
        qCritical() << "Add application failed:" << lastError;
        return false;
    }

    // 检查课题是否还有名额
    queryStr = "SELECT current_students, max_students FROM topics WHERE topic_id = ? AND status = 1";
    query.prepare(queryStr);
    query.addBindValue(applicationInfo["topic_id"].toInt());
    
    if (!query.exec() || !query.next()) {
        lastError = "Topic not available";
        qCritical() << "Add application failed:" << lastError;
        return false;
    }
    
    int current = query.value(0).toInt();
    int max = query.value(1).toInt();
    if (current >= max) {
        lastError = "Topic has no available slots";
        qCritical() << "Add application failed:" << lastError;
        return false;
    }

    // 添加申请
    queryStr = "INSERT INTO topic_applications (group_id, topic_id) VALUES (?, ?)";
    query.prepare(queryStr);
    query.addBindValue(applicationInfo["group_id"].toInt());
    query.addBindValue(applicationInfo["topic_id"].toInt());

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Add application failed:" << lastError;
        return false;
    }

    return true;
}

bool DatabaseHandler::updateApplicationStatus(int applicationId, int status, const QString& rejectReason, int reviewerId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return false;
    }

    db.transaction();

    QSqlQuery query;
    QString queryStr = "UPDATE topic_applications SET status = ?, reject_reason = ?, review_time = NOW(), reviewer_id = ? "
                      "WHERE application_id = ?";

    query.prepare(queryStr);
    query.addBindValue(status);
    query.addBindValue(rejectReason);
    query.addBindValue(reviewerId);
    query.addBindValue(applicationId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Update application status failed:" << lastError;
        db.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        lastError = "Application not found";
        db.rollback();
        return false;
    }

    // 如果申请通过，更新课题的当前人数
    if (status == 1) {
        queryStr = "SELECT topic_id FROM topic_applications WHERE application_id = ?";
        query.prepare(queryStr);
        query.addBindValue(applicationId);
        
        if (query.exec() && query.next()) {
            int topicId = query.value(0).toInt();
            queryStr = "UPDATE topics SET current_students = current_students + 1 WHERE topic_id = ?";
            query.prepare(queryStr);
            query.addBindValue(topicId);
            
            if (!query.exec()) {
                lastError = query.lastError().text();
                qCritical() << "Update topic student count failed:" << lastError;
                db.rollback();
                return false;
            }
        }
    }

    db.commit();
    return true;
}

QJsonArray DatabaseHandler::getApplications(int groupId, int topicId, int status)
{
    QMutexLocker locker(&mutex);
    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonArray();
    }

    QSqlQuery query;
    QString queryStr = "SELECT a.*, t.topic_name, u.real_name AS teacher_name, "
                       "sg.leader_name AS group_name, u2.real_name AS reviewer_name "
                       "FROM topic_applications a "
                       "LEFT JOIN topics t ON a.topic_id = t.topic_id "
                       "LEFT JOIN teachers te ON t.teacher_id = te.teacher_id "
                       "LEFT JOIN users u ON te.user_id = u.user_id "
                       "LEFT JOIN student_groups sg ON a.group_id = sg.group_id "
                       "LEFT JOIN users u2 ON a.reviewer_id = u2.user_id "
                       "WHERE 1=1";
    QVariantList params;

    if (groupId != -1) {
        queryStr += " AND a.group_id = ?";
        params << groupId;
    }
    if (topicId != -1) {
        queryStr += " AND a.topic_id = ?";
        params << topicId;
    }
    if (status != -1) {
        queryStr += " AND a.status = ?";
        params << status;
    }

    query.prepare(queryStr);
    for (const QVariant& param : params) {
        query.addBindValue(param);
    }

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get applications failed:" << lastError;
        return QJsonArray();
    }

    return queryToJsonArray(query);
}

// 其他操作实现
QJsonObject DatabaseHandler::getTeacherInfo(int teacherId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    QString queryStr = "SELECT t.*, u.username, u.real_name, u.email, u.phone "
                      "FROM teachers t JOIN users u ON t.user_id = u.user_id WHERE t.teacher_id = ?";

    query.prepare(queryStr);
    query.addBindValue(teacherId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get teacher info failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        QJsonObject teacherInfo;
        teacherInfo["teacher_id"] = query.value(0).toInt();
        teacherInfo["user_id"] = query.value(1).toInt();
        teacherInfo["department"] = query.value(2).toString();
        teacherInfo["title"] = query.value(3).toString();
        teacherInfo["research_area"] = query.value(4).toString();
        teacherInfo["username"] = query.value(5).toString();
        teacherInfo["real_name"] = query.value(6).toString();
        teacherInfo["email"] = query.value(7).toString();
        teacherInfo["phone"] = query.value(8).toString();

        return teacherInfo;
    }

    lastError = "Teacher not found";
    return QJsonObject();
}

QJsonObject DatabaseHandler::getStudentGroupInfo(int groupId)
{
    QMutexLocker locker(&mutex);

    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    QString queryStr = "SELECT sg.*, u.username, u.real_name, u.email, u.phone "
                      "FROM student_groups sg JOIN users u ON sg.user_id = u.user_id WHERE sg.group_id = ?";

    query.prepare(queryStr);
    query.addBindValue(groupId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get student group info failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        QJsonObject groupInfo;
        groupInfo["group_id"] = query.value(0).toInt();
        groupInfo["user_id"] = query.value(1).toInt();
        groupInfo["leader_name"] = query.value(2).toString();
        groupInfo["member_count"] = query.value(3).toInt();
        groupInfo["members"] = query.value(4).toString();
        groupInfo["major"] = query.value(5).toString();
        groupInfo["grade"] = query.value(6).toString();
        groupInfo["class_name"] = query.value(7).toString();
        groupInfo["username"] = query.value(8).toString();
        groupInfo["real_name"] = query.value(9).toString();
        groupInfo["email"] = query.value(10).toString();
        groupInfo["phone"] = query.value(11).toString();

        return groupInfo;
    }

    lastError = "Student group not found";
    return QJsonObject();
}

QJsonObject DatabaseHandler::getStatistics()
{
    QMutexLocker locker(&mutex);
    QJsonObject stats;
    QSqlQuery query;

    // 用户统计（原有）
    query.exec("SELECT COUNT(*) FROM users WHERE user_type = 1");
    if (query.next()) stats["admin_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM users WHERE user_type = 2");
    if (query.next()) stats["teacher_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM users WHERE user_type = 3");
    if (query.next()) stats["student_count"] = query.value(0).toInt();

    // 新增：活跃/禁用用户数
    query.exec("SELECT COUNT(*) FROM users WHERE status = 1");
    if (query.next()) stats["active_user_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM users WHERE status = 0");
    if (query.next()) stats["inactive_user_count"] = query.value(0).toInt();

    // 课题统计（原有）
    query.exec("SELECT COUNT(*) FROM topics WHERE status = 1");
    if (query.next()) stats["active_topic_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM topics WHERE status = 0");
    if (query.next()) stats["closed_topic_count"] = query.value(0).toInt();

    // 新增：总课题数、已申请课题数、已完成课题数（通过申请且课题完成？简化处理）
    query.exec("SELECT COUNT(*) FROM topics");
    if (query.next()) stats["total_topic_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(DISTINCT topic_id) FROM topic_applications");
    if (query.next()) stats["applied_topic_count"] = query.value(0).toInt();
    // 已完成课题：申请状态为1且课题状态为0（已关闭），这里简化为申请通过的数量
    query.exec("SELECT COUNT(*) FROM topic_applications WHERE status = 1");
    if (query.next()) stats["completed_topic_count"] = query.value(0).toInt();

    // 申请统计（原有，但字段名需调整）
    query.exec("SELECT COUNT(*) FROM topic_applications");
    if (query.next()) stats["total_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM topic_applications WHERE status = 0");
    if (query.next()) stats["pending_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM topic_applications WHERE status = 1");
    if (query.next()) stats["approved_count"] = query.value(0).toInt();
    query.exec("SELECT COUNT(*) FROM topic_applications WHERE status = 2");
    if (query.next()) stats["rejected_count"] = query.value(0).toInt();

    return stats;
}

QJsonObject DatabaseHandler::getApplication(int applicationId)
{
    QMutexLocker locker(&mutex);
    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM topic_applications WHERE application_id = ?");
    query.addBindValue(applicationId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get application failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        return queryToJsonObject(query);
    }

    lastError = "Application not found";
    return QJsonObject();
}

QJsonObject DatabaseHandler::getTeacherInfoByUserId(int userId)
{
    QMutexLocker locker(&mutex);
    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    query.prepare("SELECT teacher_id FROM teachers WHERE user_id = ?");
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get teacher info by user_id failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        QJsonObject obj;
        obj["teacher_id"] = query.value(0).toInt();
        return obj;
    }

    lastError = "Teacher not found for user_id";
    return QJsonObject();
}

QJsonObject DatabaseHandler::getStudentGroupInfoByUserId(int userId)
{
    QMutexLocker locker(&mutex);
    if (!db.isOpen()) {
        lastError = "Database not connected";
        return QJsonObject();
    }

    QSqlQuery query;
    query.prepare("SELECT group_id FROM student_groups WHERE user_id = ?");
    query.addBindValue(userId);

    if (!query.exec()) {
        lastError = query.lastError().text();
        qCritical() << "Get student group info by user_id failed:" << lastError;
        return QJsonObject();
    }

    if (query.next()) {
        QJsonObject obj;
        obj["group_id"] = query.value(0).toInt();
        return obj;
    }

    lastError = "Student group not found for user_id";
    return QJsonObject();
}

bool DatabaseHandler::hasDependentData(int userId)
{
    QMutexLocker locker(&mutex);
    QSqlQuery query;
    qDebug() << "=== hasDependentData called for user ID:" << userId;

    // 检查用户是否为教师且发布了课题（未关闭的）
    query.prepare("SELECT COUNT(*) FROM topics t "
                  "JOIN teachers te ON t.teacher_id = te.teacher_id "
                  "WHERE te.user_id = ? AND t.status = 1");
    query.addBindValue(userId);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "Active topics count:" << count;
        if (count > 0) {
            return true;
        }
    }

    // 检查用户是否为小组且有待审核申请（status=0）
    query.prepare("SELECT COUNT(*) FROM topic_applications a "
                  "JOIN student_groups sg ON a.group_id = sg.group_id "
                  "WHERE sg.user_id = ? AND a.status = 0");
    query.addBindValue(userId);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "Pending applications count:" << count;
        if (count > 0) {
            return true;
        }
    }

    // 检查用户是否有已通过的申请（已占用课题名额）
    query.prepare("SELECT COUNT(*) FROM topic_applications a "
                  "JOIN student_groups sg ON a.group_id = sg.group_id "
                  "WHERE sg.user_id = ? AND a.status = 1");
    query.addBindValue(userId);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "Approved applications count:" << count;
        if (count > 0) {
            return true;
        }
    }

    qDebug() << "No dependent data found";
    return false;
}

bool DatabaseHandler::hasPendingApplications(int topicId)
{
    QMutexLocker locker(&mutex);
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM topic_applications WHERE topic_id = ? AND status = 0");
    query.addBindValue(topicId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
