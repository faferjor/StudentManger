#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QDateTime>
#include <QKeyEvent>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::LoginWindow),
      networkManager(NetworkManager::getInstance()),
      loginAttempts(0)
{
    ui->setupUi(this);
    initUI();
    initConnections();
}

LoginWindow::~LoginWindow()
{
    stopLoginRequest();
    delete ui;
}

void LoginWindow::initUI()
{
    setWindowTitle(tr("大学生小组课题选题管理系统 - 登录"));
    setFixedSize(400, 300);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 设置输入框占位符
    ui->usernameLineEdit->setPlaceholderText(tr("请输入账号"));
    ui->passwordLineEdit->setPlaceholderText(tr("请输入密码"));
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // 设置按钮文本
    ui->loginButton->setText(tr("登录"));
    ui->exitButton->setText(tr("退出"));

    // 设置状态标签
    ui->statusLabel->setStyleSheet("color: red; font-size: 12px;");
    ui->statusLabel->clear();

    // 创建状态清除定时器
    statusClearTimer = new QTimer(this);
    statusClearTimer->setSingleShot(true);
    statusClearTimer->setInterval(5000); // 5秒后清除状态
    connect(statusClearTimer, &QTimer::timeout, this, &LoginWindow::clearStatus);

    // 初始化服务器连接
    networkManager.connectToServer();
}

void LoginWindow::initConnections()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &LoginWindow::onExitButtonClicked);
    
    connect(&networkManager, &NetworkManager::connectionStateChanged,
            this, &LoginWindow::onConnectionStateChanged);
    connect(&networkManager, &NetworkManager::responseReceived,
            this, &LoginWindow::onResponseReceived);
    connect(&networkManager, &NetworkManager::errorOccurred,
            this, &LoginWindow::onErrorOccurred);

    // 回车键登录
    connect(ui->usernameLineEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginButtonClicked);
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginButtonClicked);
}

void LoginWindow::onLoginButtonClicked()
{
    if (!validateInput()) {
        return;
    }

    if (networkManager.getConnectionState() != NetworkManager::Connected) {
        showStatus(tr("正在连接服务器，请稍候..."));
        networkManager.connectToServer();
        return;
    }

    startLoginRequest();
}

void LoginWindow::onExitButtonClicked()
{
    reject();
    close();
}

void LoginWindow::onConnectionStateChanged(NetworkManager::ConnectionState state)
{
    //如果状态不是 Connected，客户端不会发送请求。
    qDebug() << "Connection state changed:" << state;

    switch (state) {
    case NetworkManager::Connected:
        showStatus(tr("服务器连接成功，可以登录"), false);
        break;
    case NetworkManager::Connecting:
        showStatus(tr("正在连接服务器..."));
        break;
    case NetworkManager::Disconnected:
        showStatus(tr("服务器连接断开，请检查网络"), true);
        break;
    }
}

void LoginWindow::onResponseReceived(const QString& requestId, const QJsonObject& response)
{
    //测试客户端链接
    qDebug() << "=== LoginWindow::onResponseReceived called, requestId:" << requestId;
    qDebug() << "Response status:" << response.value("status").toString();

    if (requestId != currentRequestId) {
        return;
    }

    stopLoginRequest();
    //最小化测试=============
    // QString status = response.value("status").toString();
    // if (status == "success") {
    //     qDebug() << "Login success, emitting signal...";
    //     // 发射一个简单的信号，不传递任何用户信息
    //     emit loginSuccess(1, 1, "test", "test");
    //     qDebug() << "After emit";
    // }
//==============
    QString status = response.value("status").toString();
    if (status == "success") {
        QJsonObject data = response.value("data").toObject();
        QJsonObject userInfo = data.value("user_info").toObject();
        
        // 新增，提取 Token 并保存
        QString token = data.value("token").toString();
        if (!token.isEmpty()) {
            networkManager.setSessionToken(token);        // 保存 token
        }

        int userId = userInfo.value("user_id").toInt();
        int userType = userInfo.value("user_type").toInt();
        QString username = userInfo.value("username").toString();
        QString realName = userInfo.value("real_name").toString();
        
        // 保存用户信息到网络管理器
        networkManager.setCurrentUser(userId, userType, username, realName);
        
        showStatus(tr("登录成功！欢迎回来，%1").arg(realName), false);
        
        // 延迟关闭登录窗口，让用户看到成功提示
        //======================
        QTimer::singleShot(1000, this, [=]() {
            // 断开所有 NetworkManager 的连接
            disconnect(&networkManager, &NetworkManager::connectionStateChanged, this, nullptr);
            disconnect(&networkManager, &NetworkManager::responseReceived, this, nullptr);
            disconnect(&networkManager, &NetworkManager::errorOccurred, this, nullptr);
            
            emit loginSuccess(userId, userType, username, realName);
            accept();
        });
        //=====测试
        // qDebug() << "About to emit loginSuccess";
        // emit loginSuccess(userId, userType, username, realName);
        // qDebug() << "After emit loginSuccess";
        // accept();
        
        loginAttempts = 0;
        currentRequestId.clear();
    } else {
        QString message = response.value("message").toString(tr("登录失败"));
        showStatus(message, true);
        loginAttempts++;
        
        if (loginAttempts >= 3) {
            ui->loginButton->setEnabled(false);
            showStatus(tr("登录失败次数过多，请1分钟后再试"), true);
            
            QTimer::singleShot(60000, this, [=]() {
                ui->loginButton->setEnabled(true);
                loginAttempts = 0;
                clearStatus();
            });
        }
        currentRequestId.clear();
    }
}

void LoginWindow::onErrorOccurred(NetworkManager::ErrorType errorType, const QString& errorMsg)
{
    if (currentRequestId.isEmpty()) {
        showStatus(errorMsg, true);
    }
}

bool LoginWindow::validateInput()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text().trimmed();

    if (username.isEmpty()) {
        showStatus(tr("请输入账号"), true);
        ui->usernameLineEdit->setFocus();
        return false;
    }

    if (password.isEmpty()) {
        showStatus(tr("请输入密码"), true);
        ui->passwordLineEdit->setFocus();
        return false;
    }

    // 检查账号格式
    if (username.length() < 3 || username.length() > 20) {
        showStatus(tr("账号长度必须在3-20位之间"), true);
        ui->usernameLineEdit->setFocus();
        return false;
    }

    // 检查密码强度
    if (password.length() < 6) {
        showStatus(tr("密码长度不能少于6位"), true);
        ui->passwordLineEdit->setFocus();
        return false;
    }

    return true;
}

void LoginWindow::showStatus(const QString& message, bool isError)
{
    ui->statusLabel->setText(message);
    if (isError) {
        ui->statusLabel->setStyleSheet("color: #dc3545; font-size: 12px;");
    } else {
        ui->statusLabel->setStyleSheet("color: #28a745; font-size: 12px;");
    }
    statusClearTimer->start();
}

void LoginWindow::clearStatus()
{
    ui->statusLabel->clear();
}

void LoginWindow::resetLoginForm()
{
    ui->passwordLineEdit->clear();
    ui->usernameLineEdit->setFocus();
    clearStatus();
}

void LoginWindow::startLoginRequest()
{
    if (!currentRequestId.isEmpty()) {
        stopLoginRequest();
    }

    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text().trimmed();

    QJsonObject data;
    data["username"] = username;
    data["password"] = password;

    //确认登录按钮点击后真正发送了请求
    qDebug() << "Preparing to send login request...";
    currentRequestId = networkManager.sendRequest("USER_LOGIN", data);
    qDebug() << "Request sent, ID:" << currentRequestId;

    if (currentRequestId.isEmpty()) {
        showStatus(tr("请求发送失败，请检查网络连接"), true);
    } else {
        showStatus(tr("正在登录，请稍候..."));
        qDebug()<<"发送登录请求中";
        ui->loginButton->setEnabled(false);
    }
}

void LoginWindow::stopLoginRequest()
{
    currentRequestId.clear();
    ui->loginButton->setEnabled(true);
}
void LoginWindow::on_loginButton_clicked()
{

}

