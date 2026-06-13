#include "clienthandler.h"
#include "databasehandler.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>
#include <QUuid>
#include <QMutex>

// 全局会话存储（实际项目建议用 Redis 或数据库）
static QHash<QString, int> sessionMap;
static QMutex sessionMutex;   //保护 sessionMap

// ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
//     : QObject(parent), socketDescriptor(socketDescriptor), currentUserId(0)
// {
//     socket = new QTcpSocket(this);
//     if (!socket->setSocketDescriptor(socketDescriptor)) {
//         emit finished();
//         return;
//     }
//     connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
//     connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
//     connect(socket, &QTcpSocket::errorOccurred, this, &ClientHandler::onErrorOccurred);
// }

//测试
ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent)
    : QObject(parent), socket(socket), currentUserId(0)
{
    // 直接连接 socket 信号，不再调用 setSocketDescriptor
    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientHandler::onErrorOccurred);
}



ClientHandler::~ClientHandler()
{
    //socket->deleteLater();
    // 注意：不要 delete socket，它会在连接关闭后由上层（QTcpServer）管理
}

void ClientHandler::onReadyRead()
{
    QByteArray data = socket->readAll();
    //服务端是否真的收到了数据
    qDebug() << "Server received raw data:" << data;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        sendErrorResponse("", "Invalid JSON format");
        return;
    }

    if (!doc.isObject()) {
        sendErrorResponse("", "Request must be a JSON object");
        return;
    }

    QJsonObject request = doc.object();
    QString requestId = request.value("request_id").toString();
    QString command = request.value("command").toString();
    QJsonObject dataObj = request.value("data").toObject();

    if (command.isEmpty()) {
        sendErrorResponse(requestId, "Command is required");
        return;
    }

    handleCommand(requestId, command, dataObj, request);
}

void ClientHandler::onDisconnected()
{
    qInfo() << "Client disconnected:" << socketDescriptor;
    emit finished();
}

void ClientHandler::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    //qWarning() << "Client error on socket" << socketDescriptor << ":" << socket->errorString();
    qWarning() << "Client error:" << socket->errorString();
    emit finished();
}

void ClientHandler::handleCommand(const QString& requestId, const QString& command,
                                  const QJsonObject& data, const QJsonObject& fullRequest)
{
    //确认命令解析和分发
    qDebug() << "Command received:" << command;

    // 登录命令不需要 token
    if (command != "USER_LOGIN") {
        int userId = 0;
        if (!validateToken(fullRequest, userId)) {
            sendErrorResponse(requestId, "Unauthorized: Invalid or missing token");
            return;
        }
        currentUserId = userId;
    }

    // 分发命令
    if (command == "USER_LOGIN") {
        handleLogin(requestId, data);
    } else if (command == "GET_USER_INFO") {
        handleGetUserInfo(requestId, data);
    } else if (command == "GET_USERS") {
        handleGetUsers(requestId, data);
    } else if (command == "ADD_USER") {
        handleAddUser(requestId, data);
    } else if (command == "UPDATE_USER") {
        handleUpdateUser(requestId, data);
    } else if (command == "DELETE_USER") {
        handleDeleteUser(requestId, data);
    } else if (command == "ADD_TOPIC") {
        handleAddTopic(requestId, data);
    } else if (command == "UPDATE_TOPIC") {
        handleUpdateTopic(requestId, data);
    } else if (command == "DELETE_TOPIC") {
        handleDeleteTopic(requestId, data);
    } else if (command == "GET_TOPIC") {
        handleGetTopic(requestId, data);
    } else if (command == "GET_TOPICS") {
        handleGetTopics(requestId, data);
    } else if (command == "ADD_APPLICATION") {
        handleAddApplication(requestId, data);
    } else if (command == "UPDATE_APPLICATION_STATUS") {
        handleUpdateApplicationStatus(requestId, data);
    } else if (command == "GET_APPLICATIONS") {
        handleGetApplications(requestId, data);
    } else if (command == "GET_STATISTICS") {
        handleGetStatistics(requestId);
    } else {
        sendErrorResponse(requestId, QString("Unknown command: %1").arg(command));
    }
}

bool ClientHandler::validateToken(const QJsonObject& fullRequest, int& userId)
{
    QString token = fullRequest.value("token").toString();
    if (token.isEmpty()) return false;
    // 使用互斥锁保护 sessionMap
    QMutexLocker locker(&sessionMutex);
    if (sessionMap.contains(token)) {
        userId = sessionMap[token];
        return true;
    }
    return false;
}

// ------------------- 业务处理函数（含权限检查） -------------------

// 注意：所有数据库操作都是通过 DatabaseHandler::getInstance() 调用，且现在所有代码都在主线程，
// 因此不会再有跨线程错误。

void ClientHandler::handleLogin(const QString& requestId, const QJsonObject& data)
{
    //确认业务处理是否执行
    qDebug() << "=== handleLogin called ===";

    QString username = data.value("username").toString();
    QString password = data.value("password").toString();

    if (username.isEmpty() || password.isEmpty()) {
        sendErrorResponse(requestId, "Username and password are required");
        return;
    }

    QJsonObject userInfo = DatabaseHandler::getInstance()->validateUser(username, password);
    if (userInfo.isEmpty()) {
        sendErrorResponse(requestId, "Invalid username or password");
        return;
    }

    int userId = userInfo.value("user_id").toInt();
    QJsonObject fullUserInfo = DatabaseHandler::getInstance()->getUserInfo(userId);

    // 生成会话 token
    QString token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    //添加 sessionMap 的互斥锁
    static QMutex sessionMutex;
    QMutexLocker locker(&sessionMutex);
    sessionMap[token] = userId;

    QJsonObject response;
    response["status"] = "success";
    response["message"] = "Login successful";
    QJsonObject responseData;
    responseData["user_info"] = fullUserInfo;
    responseData["token"] = token;
    response["data"] = responseData;

    sendResponse(requestId, response);
}

void ClientHandler::handleGetUserInfo(const QString& requestId, const QJsonObject& data)
{
    int targetUserId = data.value("user_id").toInt();
    if (targetUserId <= 0) {
        sendErrorResponse(requestId, "Invalid user ID");
        return;
    }

    // 权限：只能查自己的信息，或者管理员可以查任意
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();
    if (currentType != 1 && targetUserId != currentUserId) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    QJsonObject userInfo = DatabaseHandler::getInstance()->getUserInfo(targetUserId);
    if (userInfo.isEmpty()) {
        sendErrorResponse(requestId, "User not found");
        return;
    }

    QJsonObject response;
    response["status"] = "success";
    response["message"] = "User info retrieved";
    QJsonObject responseData;
    responseData["user_info"] = userInfo;
    response["data"] = responseData;
    sendResponse(requestId, response);
}

void ClientHandler::handleGetUsers(const QString& requestId, const QJsonObject& data)
{
    qDebug() << "=== handleGetUsers called ===";
    qDebug() << "Received data:" << QJsonDocument(data).toJson(QJsonDocument::Compact);

    // 只有管理员可以获取用户列表
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    if (currentUser.value("user_type").toInt() != 1) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    qDebug() << "Extracted actualData:" << QJsonDocument(actualData).toJson(QJsonDocument::Compact);

    int userType = actualData.value("user_type").toInt(-1);
    QJsonArray users = DatabaseHandler::getInstance()->getUsers(userType);
    qDebug() << "Found" << users.size() << "users";

    QJsonObject response;
    response["status"] = "success";
    QJsonObject responseData;
    responseData["users"] = users;
    response["data"] = responseData;
    sendResponse(requestId, response);
}

void ClientHandler::handleAddUser(const QString& requestId, const QJsonObject& data)
{
    qDebug() << "=== handleAddUser called ===";
    qDebug() << "Received data:" << QJsonDocument(data).toJson(QJsonDocument::Compact);

    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    if (currentUser.value("user_type").toInt() != 1) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    // 从嵌套结构中提取实际的用户数据
    QJsonObject userData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        userData = data.value("data").toObject();
    }

    qDebug() << "Extracted userData:" << QJsonDocument(userData).toJson(QJsonDocument::Compact);

    bool success = DatabaseHandler::getInstance()->addUser(userData);
    QString errorMsg = DatabaseHandler::getInstance()->getLastError();
    qDebug() << "Add user result:" << success << ", error:" << errorMsg;

    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "User added" : ("Failed to add user: " + errorMsg);
    sendResponse(requestId, response);
}

void ClientHandler::handleUpdateUser(const QString& requestId, const QJsonObject& data)
{
    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int targetUserId = actualData.value("user_id").toInt();
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();

    if (currentType != 1 && targetUserId != currentUserId) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    bool success = DatabaseHandler::getInstance()->updateUserInfo(actualData);
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "User updated" : "Failed to update user";
    sendResponse(requestId, response);
}

void ClientHandler::handleDeleteUser(const QString& requestId, const QJsonObject& data)
{
    qDebug() << "=== handleDeleteUser called ===";
    qDebug() << "Received data:" << QJsonDocument(data).toJson(QJsonDocument::Compact);

    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    if (currentUser.value("user_type").toInt() != 1) {
        qDebug() << "Permission denied";
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    qDebug() << "Extracted actualData:" << QJsonDocument(actualData).toJson(QJsonDocument::Compact);

    int userId = actualData.value("user_id").toInt();
    qDebug() << "Deleting user with ID:" << userId;

    //检查用户是否有课题或申请
    if (DatabaseHandler::getInstance()->hasDependentData(userId)) {
        qDebug() << "User has dependent data, cannot delete";
        sendErrorResponse(requestId, "User has dependent topics or pending applications, cannot delete");
        return;
    }

    bool success = DatabaseHandler::getInstance()->deleteUser(userId);
    QString errorMsg = DatabaseHandler::getInstance()->getLastError();
    qDebug() << "Delete user result:" << success << ", error:" << errorMsg;
    
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "User deleted" : ("Failed to delete user: " + errorMsg);
    sendResponse(requestId, response);
}

void ClientHandler::handleAddTopic(const QString& requestId, const QJsonObject& data)
{
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();
    if (currentType != 1 && currentType != 2) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    // 从嵌套结构中提取实际的数据
    QJsonObject topicData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        topicData = data.value("data").toObject();
    }

    // 教师只能添加自己的课题，强制设置 teacher_id
    if (currentType == 2) {
        QJsonObject teacherInfo = DatabaseHandler::getInstance()->getTeacherInfoByUserId(currentUserId);
        int teacherId = teacherInfo.value("teacher_id").toInt();
        if (teacherId <= 0) {
            sendErrorResponse(requestId, "Teacher profile not found");
            return;
        }
        topicData["teacher_id"] = teacherId;
    }

    bool success = DatabaseHandler::getInstance()->addTopic(topicData);
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "Topic added" : "Failed to add topic";
    sendResponse(requestId, response);
}

void ClientHandler::handleUpdateTopic(const QString& requestId, const QJsonObject& data)
{
    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int topicId = actualData.value("topic_id").toInt();
    QJsonObject topic = DatabaseHandler::getInstance()->getTopic(topicId);
    if (topic.isEmpty()) {
        sendErrorResponse(requestId, "Topic not found");
        return;
    }

    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();
    int topicTeacherId = topic.value("teacher_id").toInt();

    // 权限：管理员 或 课题的指导教师
    if (currentType != 1) {
        if (currentType == 2) {
            QJsonObject teacherInfo = DatabaseHandler::getInstance()->getTeacherInfoByUserId(currentUserId);
            if (teacherInfo.value("teacher_id").toInt() != topicTeacherId) {
                sendErrorResponse(requestId, "Permission denied");
                return;
            }
        } else {
            sendErrorResponse(requestId, "Permission denied");
            return;
        }
    }

    bool success = DatabaseHandler::getInstance()->updateTopic(actualData);
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "Topic updated" : "Failed to update topic";
    sendResponse(requestId, response);
}

void ClientHandler::handleDeleteTopic(const QString& requestId, const QJsonObject& data)
{
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    if (currentUser.value("user_type").toInt() != 1) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int topicId = actualData.value("topic_id").toInt();
    if (DatabaseHandler::getInstance()->hasPendingApplications(topicId)) {
        sendErrorResponse(requestId, "Topic has pending applications, cannot delete");
        return;
    }

    bool success = DatabaseHandler::getInstance()->deleteTopic(topicId);
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "Topic deleted" : "Failed to delete topic";
    sendResponse(requestId, response);
}

void ClientHandler::handleGetTopic(const QString& requestId, const QJsonObject& data)
{
    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int topicId = actualData.value("topic_id").toInt();
    if (topicId <= 0) {
        sendErrorResponse(requestId, "Invalid topic ID");
        return;
    }

    QJsonObject topic = DatabaseHandler::getInstance()->getTopic(topicId);
    if (topic.isEmpty()) {
        sendErrorResponse(requestId, "Topic not found");
        return;
    }

    QJsonObject response;
    response["status"] = "success";
    response["message"] = "Topic retrieved";
    QJsonObject responseData;
    responseData["topic"] = topic;
    response["data"] = responseData;
    sendResponse(requestId, response);
}

void ClientHandler::handleGetTopics(const QString& requestId, const QJsonObject& data)
{
    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int status = actualData.value("status").toInt(-1);
    int teacherId = actualData.value("teacher_id").toInt(-1);

    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();
    // 如果是教师，强制只查询自己的课题
    if (currentType == 2) {
        QJsonObject teacherInfo = DatabaseHandler::getInstance()->getTeacherInfoByUserId(currentUserId);
        teacherId = teacherInfo.value("teacher_id").toInt();
    }

    QJsonArray topics = DatabaseHandler::getInstance()->getTopics(status, teacherId);
    QJsonObject response;
    response["status"] = "success";
    QJsonObject responseData;
    responseData["topics"] = topics;
    response["data"] = responseData;
    sendResponse(requestId, response);
}

void ClientHandler::handleAddApplication(const QString& requestId, const QJsonObject& data)
{
    // 只有学生小组可以申请
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    if (currentUser.value("user_type").toInt() != 3) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    // 获取小组ID
    QJsonObject groupInfo = DatabaseHandler::getInstance()->getStudentGroupInfoByUserId(currentUserId);
    int groupId = groupInfo.value("group_id").toInt();
    if (groupId <= 0) {
        sendErrorResponse(requestId, "Student group profile not found");
        return;
    }

    // 从嵌套结构中提取实际的数据
    QJsonObject appData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        appData = data.value("data").toObject();
    }
    appData["group_id"] = groupId;

    bool success = DatabaseHandler::getInstance()->addApplication(appData);
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "Application submitted" : "Failed to submit application";
    sendResponse(requestId, response);
}

void ClientHandler::handleUpdateApplicationStatus(const QString& requestId, const QJsonObject& data)
{
    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int applicationId = actualData.value("application_id").toInt();
    if (applicationId <= 0) {
        sendErrorResponse(requestId, "Invalid application ID");
        return;
    }

    // 获取申请信息
    QJsonObject app = DatabaseHandler::getInstance()->getApplication(applicationId);
    if (app.isEmpty()) {
        sendErrorResponse(requestId, "Application not found");
        return;
    }

    int topicId = app.value("topic_id").toInt();
    QJsonObject topic = DatabaseHandler::getInstance()->getTopic(topicId);
    int teacherId = topic.value("teacher_id").toInt();

    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();

    // 权限：管理员 或 课题的指导教师
    if (currentType != 1) {
        if (currentType == 2) {
            QJsonObject teacherInfo = DatabaseHandler::getInstance()->getTeacherInfoByUserId(currentUserId);
            if (teacherInfo.value("teacher_id").toInt() != teacherId) {
                sendErrorResponse(requestId, "Permission denied");
                return;
            }
        } else {
            sendErrorResponse(requestId, "Permission denied");
            return;
        }
    }

    int status = actualData.value("status").toInt();
    QString rejectReason = actualData.value("reject_reason").toString();

    bool success = DatabaseHandler::getInstance()->updateApplicationStatus(applicationId, status, rejectReason, currentUserId);
    QJsonObject response;
    response["status"] = success ? "success" : "fail";
    response["message"] = success ? "Application status updated" : "Failed to update";
    sendResponse(requestId, response);
}

void ClientHandler::handleGetApplications(const QString& requestId, const QJsonObject& data)
{
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();

    // 从嵌套结构中提取实际的数据
    QJsonObject actualData = data;
    if (data.contains("data") && data.value("data").isObject()) {
        actualData = data.value("data").toObject();
    }

    int groupId = actualData.value("group_id").toInt(-1);
    int topicId = actualData.value("topic_id").toInt(-1);
    int status = actualData.value("status").toInt(-1);

    // 学生小组只能查自己小组的申请
    if (currentType == 3) {
        QJsonObject groupInfo = DatabaseHandler::getInstance()->getStudentGroupInfoByUserId(currentUserId);
        groupId = groupInfo.value("group_id").toInt();
        if (groupId <= 0) {
            sendErrorResponse(requestId, "Group profile not found");
            return;
        }
        // 强制覆盖传入的 group_id
    }
    // 教师可以查自己指导的课题的申请（可以通过 topicId 过滤，这里不额外限制，由数据库查询自行处理）
    // 管理员可以查所有

    QJsonArray applications = DatabaseHandler::getInstance()->getApplications(groupId, topicId, status);
    QJsonObject response;
    response["status"] = "success";
    response["message"] = "Applications retrieved";
    QJsonObject responseData;
    responseData["applications"] = applications;
    response["data"] = responseData;
    sendResponse(requestId, response);
}

void ClientHandler::handleGetStatistics(const QString& requestId)
{
    // 统计信息只允许管理员和教师查看
    QJsonObject currentUser = DatabaseHandler::getInstance()->getUserInfo(currentUserId);
    int currentType = currentUser.value("user_type").toInt();
    if (currentType != 1 && currentType != 2) {
        sendErrorResponse(requestId, "Permission denied");
        return;
    }

    QJsonObject stats = DatabaseHandler::getInstance()->getStatistics();
    QJsonObject response;
    response["status"] = "success";
    response["message"] = "Statistics retrieved";
    QJsonObject responseData;
    responseData["statistics"] = stats;
    response["data"] = responseData;
    sendResponse(requestId, response);
}

void ClientHandler::sendResponse(const QString& requestId, const QJsonObject& responseObj)
{
    QJsonObject response = responseObj;
    if (!requestId.isEmpty()) {
        response["request_id"] = requestId;
    }
    QJsonDocument doc(response);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";

    //确认响应是否被发送
    qDebug() << "Sending response:" << data;

    socket->write(data);
    socket->flush();
}

void ClientHandler::sendErrorResponse(const QString& requestId, const QString& message)
{
    QJsonObject response;
    response["status"] = "fail";
    response["message"] = message;
    if (!requestId.isEmpty()) {
        response["request_id"] = requestId;
    }
    sendResponse(requestId, response);
}
