#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QMutex>
#include <QMap>
#include <QDateTime>
#include <QDebug>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    enum ConnectionState {
        Disconnected,
        Connecting,
        Connected
    };

    enum ErrorType {
        NoError,
        ConnectionError,
        NetworkError,
        TimeoutError,
        ServerError,
        ParseError
    };

    static NetworkManager& getInstance()
    {
        static NetworkManager instance;
        return instance;
    }

    // 禁止拷贝构造和赋值操作
    NetworkManager(const NetworkManager&) = delete;
    void operator=(const NetworkManager&) = delete;

    // 连接服务器
    void connectToServer(const QString& hostName = "localhost", quint16 port = 12345);

    // 断开连接
    void disconnectFromServer();

    // 检查连接状态
    ConnectionState getConnectionState() const;

    // 发送请求
    QString sendRequest(const QString& command, const QJsonObject& data = QJsonObject());

    // 发送请求（带用户信息）
    QString sendRequestWithUserInfo(const QString& command, const QJsonObject& data = QJsonObject());

    // 设置当前用户信息
    void setCurrentUser(int userId, int userType, const QString& username);

    // 获取当前用户信息
    QJsonObject getCurrentUser() const;

    // 清除当前用户信息
    void clearCurrentUser();

    // 设置超时时间（毫秒）
    void setTimeout(int timeoutMs);

    // 获取错误信息
    QString getLastError() const;
    ErrorType getLastErrorType() const;

signals:
    void connectionStateChanged(ConnectionState state);
    void errorOccurred(ErrorType errorType, const QString& errorMsg);
    void requestReceived(const QJsonObject& response);
    void responseReceived(const QString& requestId, const QJsonObject& response);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    void onRequestTimeout();

private:
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void resetConnection();
    void processResponse(const QJsonObject& response);
    QString generateRequestId();
    QJsonObject createRequest(const QString& command, const QJsonObject& data = QJsonObject());
    bool validateResponse(const QJsonObject& response);

    QTcpSocket* socket;
    ConnectionState connectionState;
    QByteArray buffer;
    QMutex mutex;
    int timeoutMs;

    // 用户信息
    int currentUserId;
    int currentUserType;
    QString currentUsername;

    // 请求管理
    QMap<QString, QDateTime> pendingRequests;
    QMap<QString, QTimer*> timeoutTimers;
    QString lastError;
    ErrorType lastErrorType;
};

// 请求结果回调类型
typedef std::function<void(bool success, const QJsonObject& response)> RequestCallback;

#endif // NETWORKMANAGER_H