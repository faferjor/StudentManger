#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "networkmanager.h"
#include "mainwindow.h"
#include <QDateTime>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QComboBox>

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
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterButtonClicked);
    
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

void LoginWindow::onRegisterButtonClicked()
{
    QDialog* registerDialog = new QDialog(this);
    registerDialog->setWindowTitle(tr("用户注册"));
    registerDialog->setMinimumWidth(400);
    registerDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(registerDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    // 用户名
    QHBoxLayout* usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(new QLabel(tr("用户名:")));
    QLineEdit* usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText(tr("请输入用户名"));
    usernameLayout->addWidget(usernameEdit);
    dialogLayout->addLayout(usernameLayout);

    // 密码
    QHBoxLayout* passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(new QLabel(tr("密码:")));
    QLineEdit* passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(tr("请输入密码（至少6位）"));
    passwordLayout->addWidget(passwordEdit);
    dialogLayout->addLayout(passwordLayout);

    // 确认密码
    QHBoxLayout* confirmLayout = new QHBoxLayout();
    confirmLayout->addWidget(new QLabel(tr("确认密码:")));
    QLineEdit* confirmEdit = new QLineEdit();
    confirmEdit->setEchoMode(QLineEdit::Password);
    confirmEdit->setPlaceholderText(tr("请再次输入密码"));
    confirmLayout->addWidget(confirmEdit);
    dialogLayout->addLayout(confirmLayout);

    // 真实姓名
    QHBoxLayout* realNameLayout = new QHBoxLayout();
    realNameLayout->addWidget(new QLabel(tr("真实姓名:")));
    QLineEdit* realNameEdit = new QLineEdit();
    realNameEdit->setPlaceholderText(tr("请输入真实姓名"));
    realNameLayout->addWidget(realNameEdit);
    dialogLayout->addLayout(realNameLayout);

    // 用户类型
    QHBoxLayout* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel(tr("用户类型:")));
    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItem(tr("学生"), 3);
    typeCombo->addItem(tr("教师"), 2);
    typeLayout->addWidget(typeCombo);
    dialogLayout->addLayout(typeLayout);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton* cancelBtn = new QPushButton(tr("取消"));
    QPushButton* registerBtn = new QPushButton(tr("注册"));
    registerBtn->setStyleSheet("background-color: #28a745; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(registerBtn);
    dialogLayout->addLayout(buttonLayout);

    // 连接信号
    connect(cancelBtn, &QPushButton::clicked, registerDialog, &QDialog::reject);
    connect(registerBtn, &QPushButton::clicked, this, [=]() {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();
        QString confirm = confirmEdit->text().trimmed();
        QString realName = realNameEdit->text().trimmed();
        int userType = typeCombo->currentData().toInt();

        if (username.isEmpty() || password.isEmpty() || realName.isEmpty()) {
            QMessageBox::warning(registerDialog, tr("输入错误"), tr("所有字段不能为空"));
            return;
        }

        if (password != confirm) {
            QMessageBox::warning(registerDialog, tr("输入错误"), tr("两次输入的密码不一致"));
            return;
        }

        if (password.length() < 6) {
            QMessageBox::warning(registerDialog, tr("输入错误"), tr("密码长度不能少于6位"));
            return;
        }

        if (username.length() < 3 || username.length() > 20) {
            QMessageBox::warning(registerDialog, tr("输入错误"), tr("用户名长度必须在3-20位之间"));
            return;
        }

        // 发送注册请求
        QJsonObject request;
        request["username"] = username;
        request["password"] = password;
        request["real_name"] = realName;
        request["user_type"] = userType;

        QString requestId = networkManager.sendRequest("USER_REGISTER", request);
        
        // 等待注册结果
        QObject::connect(&networkManager, &NetworkManager::responseReceived, this, [=](const QString& reqId, const QJsonObject& response) mutable {
            if (reqId == requestId) {
                QString status = response.value("status").toString();
                if (status == "success") {
                    QMessageBox::information(registerDialog, tr("注册成功"), tr("注册成功，请使用新账号登录"));
                    registerDialog->accept();
                } else {
                    QString message = response.value("message").toString(tr("注册失败"));
                    QMessageBox::warning(registerDialog, tr("注册失败"), message);
                }
                // 断开连接
                QObject::disconnect(&networkManager, &NetworkManager::responseReceived, this, nullptr);
            }
        });
    });

    registerDialog->exec();
}

void LoginWindow::onConnectionStateChanged(NetworkManager::ConnectionState state)
{
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
    if (requestId != currentRequestId) {
        return;
    }

    stopLoginRequest();

    QString status = response.value("status").toString();
    if (status == "success") {
        QJsonObject data = response.value("data").toObject();
        QJsonObject userInfo = data.value("user_info").toObject();
        
        int userId = userInfo.value("user_id").toInt();
        int userType = userInfo.value("user_type").toInt();
        QString username = userInfo.value("username").toString();
        QString realName = userInfo.value("real_name").toString();
        
        // 保存用户信息到网络管理器
        networkManager.setCurrentUser(userId, userType, username);
        
        showStatus(tr("登录成功！欢迎回来，%1").arg(realName), false);
        
        // 延迟关闭登录窗口，让用户看到成功提示
        QTimer::singleShot(1000, this, [=]() {
            emit loginSuccess(userId, userType, username, realName);
            accept();
        });
        
        loginAttempts = 0;
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

    qDebug() << "用户名:" << username << "密码:" << password;

    QJsonObject request;
    request["username"] = username;
    request["password"] = password;

    // 手动构建请求JSON
    QJsonObject fullRequest;
    fullRequest["command"] = "USER_LOGIN";
    fullRequest["request_id"] = QString("req_%1").arg(QDateTime::currentMSecsSinceEpoch());
    fullRequest["data"] = request;

    QJsonDocument doc(fullRequest);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    qDebug() << "完整请求JSON:" << jsonData;

    currentRequestId = networkManager.sendRequest("USER_LOGIN", request);

    qDebug() << "请求ID:" << currentRequestId;
    
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

