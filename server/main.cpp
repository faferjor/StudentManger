#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QThread>
#include "databasehandler.h"

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket *socket, QObject *parent = nullptr)
        : QObject(parent), socket(socket)
    {
        connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
        connect(socket, &QTcpSocket::errorOccurred, this, &ClientHandler::onErrorOccurred);
    }

private slots:
    void onReadyRead()
    {
        QByteArray data = socket->readAll();
        qInfo() << "Received data from client" << socket->socketDescriptor() << ":" << data.trimmed();
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

        handleCommand(requestId, command, dataObj);
    }

    void onDisconnected()
    {
        qInfo() << "Client disconnected:" << socket->socketDescriptor();
        socket->deleteLater();
        deleteLater();
    }

    void onErrorOccurred(QAbstractSocket::SocketError socketError)
    {
        qWarning() << "Client error:" << socket->socketDescriptor() << "-" << socketError;
        socket->deleteLater();
        deleteLater();
    }

private:
    QTcpSocket *socket;

    void handleCommand(const QString& requestId, const QString& command, const QJsonObject& data)
    {
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

    void handleLogin(const QString& requestId, const QJsonObject& data)
    {

        qDebug()<<"接收到请求";
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

        // 获取完整用户信息
        int userId = userInfo.value("user_id").toInt();
        QJsonObject fullUserInfo = DatabaseHandler::getInstance()->getUserInfo(userId);

        QJsonObject response;
        response["status"] = "success";
        response["message"] = "Login successful";
        
        QJsonObject responseData;
        responseData["user_info"] = fullUserInfo;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void handleGetUserInfo(const QString& requestId, const QJsonObject& data)
    {
        int userId = data.value("user_id").toInt();
        
        if (userId <= 0) {
            sendErrorResponse(requestId, "Invalid user ID");
            return;
        }

        QJsonObject userInfo = DatabaseHandler::getInstance()->getUserInfo(userId);

        if (userInfo.isEmpty()) {
            sendErrorResponse(requestId, "User not found");
            return;
        }

        QJsonObject response;
        response["status"] = "success";
        response["message"] = "User info retrieved successfully";
        
        QJsonObject responseData;
        responseData["user_info"] = userInfo;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void handleGetUsers(const QString& requestId, const QJsonObject& data)
    {
        int userType = data.value("user_type").toInt(-1);
        QJsonArray users = DatabaseHandler::getInstance()->getUsers(userType);

        QJsonObject response;
        response["status"] = "success";
        response["message"] = "Users retrieved successfully";
        
        QJsonObject responseData;
        responseData["users"] = users;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void handleAddUser(const QString& requestId, const QJsonObject& data)
    {
        bool success = DatabaseHandler::getInstance()->addUser(data);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "User added successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to add user";
        }

        sendResponse(requestId, response);
    }

    void handleUpdateUser(const QString& requestId, const QJsonObject& data)
    {
        bool success = DatabaseHandler::getInstance()->updateUserInfo(data);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "User updated successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to update user";
        }

        sendResponse(requestId, response);
    }

    void handleDeleteUser(const QString& requestId, const QJsonObject& data)
    {
        int userId = data.value("user_id").toInt();
        
        if (userId <= 0) {
            sendErrorResponse(requestId, "Invalid user ID");
            return;
        }

        bool success = DatabaseHandler::getInstance()->deleteUser(userId);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "User deleted successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to delete user";
        }

        sendResponse(requestId, response);
    }

    void handleAddTopic(const QString& requestId, const QJsonObject& data)
    {
        bool success = DatabaseHandler::getInstance()->addTopic(data);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "Topic added successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to add topic";
        }

        sendResponse(requestId, response);
    }

    void handleUpdateTopic(const QString& requestId, const QJsonObject& data)
    {
        bool success = DatabaseHandler::getInstance()->updateTopic(data);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "Topic updated successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to update topic";
        }

        sendResponse(requestId, response);
    }

    void handleDeleteTopic(const QString& requestId, const QJsonObject& data)
    {
        int topicId = data.value("topic_id").toInt();
        
        if (topicId <= 0) {
            sendErrorResponse(requestId, "Invalid topic ID");
            return;
        }

        bool success = DatabaseHandler::getInstance()->deleteTopic(topicId);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "Topic deleted successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to delete topic";
        }

        sendResponse(requestId, response);
    }

    void handleGetTopic(const QString& requestId, const QJsonObject& data)
    {
        int topicId = data.value("topic_id").toInt();
        
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
        response["message"] = "Topic retrieved successfully";
        
        QJsonObject responseData;
        responseData["topic"] = topic;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void handleGetTopics(const QString& requestId, const QJsonObject& data)
    {
        int status = data.value("status").toInt(-1);
        int teacherId = data.value("teacher_id").toInt(-1);
        QJsonArray topics = DatabaseHandler::getInstance()->getTopics(status, teacherId);

        QJsonObject response;
        response["status"] = "success";
        response["message"] = "Topics retrieved successfully";
        
        QJsonObject responseData;
        responseData["topics"] = topics;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void handleAddApplication(const QString& requestId, const QJsonObject& data)
    {
        bool success = DatabaseHandler::getInstance()->addApplication(data);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "Application added successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to add application";
        }

        sendResponse(requestId, response);
    }

    void handleUpdateApplicationStatus(const QString& requestId, const QJsonObject& data)
    {
        int applicationId = data.value("application_id").toInt();
        int status = data.value("status").toInt();
        QString rejectReason = data.value("reject_reason").toString();
        int reviewerId = data.value("reviewer_id").toInt();
        
        if (applicationId <= 0) {
            sendErrorResponse(requestId, "Invalid application ID");
            return;
        }

        bool success = DatabaseHandler::getInstance()->updateApplicationStatus(applicationId, status, rejectReason, reviewerId);

        QJsonObject response;
        if (success) {
            response["status"] = "success";
            response["message"] = "Application status updated successfully";
        } else {
            response["status"] = "fail";
            response["message"] = "Failed to update application status";
        }

        sendResponse(requestId, response);
    }

    void handleGetApplications(const QString& requestId, const QJsonObject& data)
    {
        int groupId = data.value("group_id").toInt(-1);
        int topicId = data.value("topic_id").toInt(-1);
        int status = data.value("status").toInt(-1);
        QJsonArray applications = DatabaseHandler::getInstance()->getApplications(groupId, topicId, status);

        QJsonObject response;
        response["status"] = "success";
        response["message"] = "Applications retrieved successfully";
        
        QJsonObject responseData;
        responseData["applications"] = applications;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void handleGetStatistics(const QString& requestId)
    {
        QJsonObject stats = DatabaseHandler::getInstance()->getStatistics();

        QJsonObject response;
        response["status"] = "success";
        response["message"] = "Statistics retrieved successfully";
        
        QJsonObject responseData;
        responseData["statistics"] = stats;
        response["data"] = responseData;

        sendResponse(requestId, response);
    }

    void sendResponse(const QString& requestId, const QJsonObject& responseObj)
    {
        QJsonObject response = responseObj;
        if (!requestId.isEmpty()) {
            response["request_id"] = requestId;
        }

        QJsonDocument doc(response);
        QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
        socket->write(data);
        socket->flush();
    }

    void sendErrorResponse(const QString& requestId, const QString& message)
    {
        QJsonObject response;
        response["status"] = "fail";
        response["message"] = message;
        if (!requestId.isEmpty()) {
            response["request_id"] = requestId;
        }

        sendResponse(requestId, response);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 初始化数据库连接
    DatabaseHandler* dbHandler = DatabaseHandler::getInstance();
    bool dbConnected = dbHandler->connectToDatabase("localhost", "topic_management_system", "root", "123456");
    
    if (!dbConnected) {
        qCritical() << "Failed to connect to database. Server will exit.";
        return 1;
    }

    QTcpServer server;

    quint16 port = 12345;
    if (argc > 1) {
        bool ok;
        quint16 customPort = QString(argv[1]).toUShort(&ok);
        if (ok) {
            port = customPort;
        }
    }

    if (!server.listen(QHostAddress::Any, port)) {
        qCritical() << "Failed to start server:" << server.errorString();
        return 1;
    }

    qInfo() << "Server started successfully on port:" << port;
    qInfo() << "Waiting for clients to connect...";

    QObject::connect(&server, &QTcpServer::newConnection, [&server]() {
        QTcpSocket *clientSocket = server.nextPendingConnection();
        qInfo() << "New client connected:" << clientSocket->socketDescriptor();

        ClientHandler *handler = new ClientHandler(clientSocket);
        QThread *thread = new QThread();
        handler->moveToThread(thread);
        
        QObject::connect(thread, &QThread::started, handler, &ClientHandler::deleteLater);
        QObject::connect(handler, &ClientHandler::destroyed, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        
        thread->start();
    });

    return a.exec();
}

#include "main.moc"
