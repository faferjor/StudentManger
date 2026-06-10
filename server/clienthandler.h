#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QHash>

class ClientHandler : public QObject
{
    Q_OBJECT
public:
   // explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);
    explicit ClientHandler(QTcpSocket *socket, QObject *parent = nullptr);
    ~ClientHandler();

signals:
    void finished();

private slots:
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    int currentUserId;   // 用于存储已验证的用户ID

    void handleCommand(const QString& requestId, const QString& command,
                       const QJsonObject& data, const QJsonObject& fullRequest);
    void sendResponse(const QString& requestId, const QJsonObject& response);
    void sendErrorResponse(const QString& requestId, const QString& message);

    // 各个业务处理函数（拷贝自原 ClientHandler）
    void handleLogin(const QString& requestId, const QJsonObject& data);
    void handleGetUserInfo(const QString& requestId, const QJsonObject& data);
    void handleGetUsers(const QString& requestId, const QJsonObject& data);
    void handleAddUser(const QString& requestId, const QJsonObject& data);
    void handleUpdateUser(const QString& requestId, const QJsonObject& data);
    void handleDeleteUser(const QString& requestId, const QJsonObject& data);
    void handleAddTopic(const QString& requestId, const QJsonObject& data);
    void handleUpdateTopic(const QString& requestId, const QJsonObject& data);
    void handleDeleteTopic(const QString& requestId, const QJsonObject& data);
    void handleGetTopic(const QString& requestId, const QJsonObject& data);
    void handleGetTopics(const QString& requestId, const QJsonObject& data);
    void handleAddApplication(const QString& requestId, const QJsonObject& data);
    void handleUpdateApplicationStatus(const QString& requestId, const QJsonObject& data);
    void handleGetApplications(const QString& requestId, const QJsonObject& data);
    void handleGetStatistics(const QString& requestId);

    // 业务处理函数
    bool validateToken(const QJsonObject& fullRequest, int& userId);


};

#endif // CLIENTHANDLER_H
