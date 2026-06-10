#include "networkmanager.h"
#include <QJsonParseError>
#include <QUuid>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), socket(nullptr), connectionState(Disconnected),
      timeoutMs(30000), currentUserId(0), currentUserType(0),
      lastErrorType(NoError)
{
    socket = new QTcpSocket(this);
    
    connect(socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &NetworkManager::onErrorOccurred);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
}

NetworkManager::~NetworkManager()
{
    disconnectFromServer();
    socket->deleteLater();
}

//实现设置/获取 Token：
void NetworkManager::setSessionToken(const QString& token)
{
    sessionToken = token;
}

QString NetworkManager::getSessionToken() const
{
    return sessionToken;
}


void NetworkManager::connectToServer(const QString& hostName, quint16 port)
{
    if (connectionState == Connecting || connectionState == Connected) {
        return;
    }

    connectionState = Connecting;
    emit connectionStateChanged(connectionState);

    socket->connectToHost(hostName, port);

    // 设置连接超时
    QTimer::singleShot(timeoutMs, this, [this]() {
        if (connectionState == Connecting) {
            socket->abort();
            connectionState = Disconnected;
            lastErrorType = TimeoutError;
            lastError = "Connection timed out";
            emit errorOccurred(lastErrorType, lastError);
            emit connectionStateChanged(connectionState);
        }
    });
}

void NetworkManager::disconnectFromServer()
{
    if (connectionState == Disconnected) {
        return;
    }

    socket->disconnectFromHost();
    if (socket->state() != QTcpSocket::UnconnectedState) {
        socket->waitForDisconnected(1000);
    }

    resetConnection();
}

void NetworkManager::resetConnection()
{
    connectionState = Disconnected;
    buffer.clear();

    // 清除所有待处理请求
    QMapIterator<QString, QTimer*> it(timeoutTimers);
    while (it.hasNext()) {
        it.next();
        it.value()->stop();
        it.value()->deleteLater();
    }
    timeoutTimers.clear();
    pendingCallbacks.clear();   // 原为 pendingRequests.clear()

    emit connectionStateChanged(connectionState);
}
NetworkManager::ConnectionState NetworkManager::getConnectionState() const
{
    return connectionState;
}


QString NetworkManager::sendRequest(const QString& command, const QJsonObject& data,
                                    std::function<void(bool, const QJsonObject&)> callback)
{
    if (connectionState != Connected) {
        lastErrorType = ConnectionError;
        lastError = "Not connected to server";
        emit errorOccurred(lastErrorType, lastError);
        if (callback) callback(false, QJsonObject());
        return QString();
    }

    QString requestId = generateRequestId();
    QJsonObject request = createRequest(command, data);
    request["request_id"] = requestId;
    if (!sessionToken.isEmpty()) {
        request["token"] = sessionToken;
    }

    QJsonDocument doc(request);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact) + "\n";

   // QMutexLocker locker(&mutex);
    qint64 bytesWritten = socket->write(jsonData);
    socket->flush();
    //确认客户端确实写入了 socket
    qDebug() << "Wrote" << bytesWritten << "bytes, jsonData:" << jsonData;

    if (bytesWritten != jsonData.size()) {
        lastErrorType = NetworkError;
        lastError = "Failed to send complete request";
        emit errorOccurred(lastErrorType, lastError);
        if (callback) callback(false, QJsonObject());
        return QString();
    }

    // 存储回调
    PendingRequest pending;
    pending.timestamp = QDateTime::currentDateTime();
    pending.callback = callback;
    pendingCallbacks[requestId] = pending;

    // 设置超时
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(timeoutMs);
    connect(timer, &QTimer::timeout, this, [this, requestId]() {
        if (pendingCallbacks.contains(requestId)) {
            auto cb = pendingCallbacks[requestId].callback;
            pendingCallbacks.remove(requestId);
            if (cb) cb(false, QJsonObject());
        }
        timeoutTimers.remove(requestId);
    });
    timeoutTimers[requestId] = timer;
    timer->start();

    return requestId;
}

QString NetworkManager::sendRequestWithUserInfo(const QString& command, const QJsonObject& data)
{
    if (currentUserId == 0) {
        lastErrorType = ConnectionError;
        lastError = "User not logged in";
        emit errorOccurred(lastErrorType, lastError);
        return QString();
    }

    QJsonObject requestData = data;
    
    QJsonObject userInfo;
    userInfo["user_id"] = currentUserId;
    userInfo["user_type"] = currentUserType;
    userInfo["username"] = currentUsername;
    
    // 创建完整的请求数据
    QJsonObject fullData;
    fullData["user_info"] = userInfo;
    fullData["data"] = requestData;
    
    return sendRequest(command, fullData);
}

void NetworkManager::setCurrentUser(int userId, int userType, const QString& username)
{
    //======测试
    // qDebug() << "setCurrentUser: start, userId=" << userId;
    // QMutexLocker locker(&mutex);
    qDebug() << "setCurrentUser:  locked";
    currentUserId = userId;
    currentUserType = userType;
    currentUsername = username;
    qDebug() << "setCurrentUser: end";


    // QMutexLocker locker(&mutex);
    // currentUserId = userId;
    // currentUserType = userType;
    // currentUsername = username;
}

QJsonObject NetworkManager::getCurrentUser() const
{
   // QMutexLocker locker(const_cast<QMutex*>(&mutex));
    QJsonObject userInfo;
    userInfo["user_id"] = currentUserId;
    userInfo["user_type"] = currentUserType;
    userInfo["username"] = currentUsername;
    return userInfo;
}

void NetworkManager::clearCurrentUser()
{
   // QMutexLocker locker(&mutex);
    currentUserId = 0;
    currentUserType = 0;
    currentUsername.clear();

    //退出登录时清除 Token
    setSessionToken("");
}

void NetworkManager::setTimeout(int timeoutMs)
{
    this->timeoutMs = timeoutMs;
}

QString NetworkManager::getLastError() const
{
    //QMutexLocker locker(const_cast<QMutex*>(&mutex));
    return lastError;
}

NetworkManager::ErrorType NetworkManager::getLastErrorType() const
{
    //QMutexLocker locker(const_cast<QMutex*>(&mutex));
    return lastErrorType;
}

void NetworkManager::onConnected()
{
    connectionState = Connected;
    lastErrorType = NoError;
    lastError.clear();
    
    emit connectionStateChanged(connectionState);
    qDebug() << "Connected to server successfully";
}

void NetworkManager::onDisconnected()
{
    resetConnection();
    qDebug() << "Disconnected from server";
}

void NetworkManager::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    
    lastErrorType = NetworkError;
    lastError = socket->errorString();
    
    emit errorOccurred(lastErrorType, lastError);
    qDebug() << "Network error:" << lastError;
    
    if (connectionState == Connecting) {
        connectionState = Disconnected;
        emit connectionStateChanged(connectionState);
    }
}

void NetworkManager::onReadyRead()
{
   //QMutexLocker locker(&mutex);
    buffer.append(socket->readAll());

    // 处理完整的JSON数据包（假设每个包以"\n"结尾）
    int newlineIndex = buffer.indexOf('\n');
    while (newlineIndex != -1) {
        QByteArray data = buffer.left(newlineIndex).trimmed();
        buffer = buffer.mid(newlineIndex + 1);

        if (!data.isEmpty()) {
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) {
                    QJsonObject response = jsonDoc.object();
                    emit requestReceived(response);
                    processResponse(response);
                } else {
                    lastErrorType = ParseError;
                    lastError = "Invalid JSON object received";
                    emit errorOccurred(lastErrorType, lastError);
                }
            } else {
                lastErrorType = ParseError;
                lastError = "JSON parse error: " + parseError.errorString();
                emit errorOccurred(lastErrorType, lastError);
            }
        }

        newlineIndex = buffer.indexOf('\n');
    }
}

void NetworkManager::processResponse(const QJsonObject& response)
{
    QString requestId = response.value("request_id").toString();

    qDebug() << "NetworkManager::processResponse, requestId:" << requestId;

    if (!requestId.isEmpty() && pendingCallbacks.contains(requestId)) {
        auto cb = pendingCallbacks[requestId].callback;
        pendingCallbacks.remove(requestId);
        if (timeoutTimers.contains(requestId)) {
            timeoutTimers[requestId]->stop();
            timeoutTimers[requestId]->deleteLater();
            timeoutTimers.remove(requestId);
        }

        QString status = response.value("status").toString();
        bool success = (status == "success");
        if (cb) cb(success, response);
    }

    emit responseReceived(requestId, response);
}

QString NetworkManager::generateRequestId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QJsonObject NetworkManager::createRequest(const QString& command, const QJsonObject& data)
{
    QJsonObject request;
    request["command"] = command;
    request["data"] = data;
    request["timestamp"] = QDateTime::currentDateTime().toSecsSinceEpoch();
    return request;
}

bool NetworkManager::validateResponse(const QJsonObject& response)
{
    if (!response.contains("request_id") || response.value("request_id").toString().isEmpty()) {
        return false;
    }
    
    if (!response.contains("status") || !response.contains("message")) {
        return false;
    }
    
    return true;
}

void NetworkManager::onRequestTimeout()
{
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if (timer) {
        QString requestId;
        QMapIterator<QString, QTimer*> it(timeoutTimers);
        while (it.hasNext()) {
            it.next();
            if (it.value() == timer) {
                requestId = it.key();
                break;
            }
        }
        
        if (!requestId.isEmpty()) {
            pendingCallbacks.remove(requestId);
            timeoutTimers.remove(requestId);
            
            lastErrorType = TimeoutError;
            lastError = "Request timed out: " + requestId;
            emit errorOccurred(lastErrorType, lastError);
            
            QJsonObject response;
            response["request_id"] = requestId;
            response["status"] = "fail";
            response["message"] = lastError;
            emit responseReceived(requestId, response);
        }
        
        timer->deleteLater();
    }
}

void NetworkManager::cancelRequest(const QString& requestId)
{
    if (pendingCallbacks.contains(requestId)) {
        pendingCallbacks.remove(requestId);
    }
    if (timeoutTimers.contains(requestId)) {
        timeoutTimers[requestId]->stop();
        timeoutTimers[requestId]->deleteLater();
        timeoutTimers.remove(requestId);
    }
}
