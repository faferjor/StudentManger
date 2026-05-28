#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkmanager.h"
#include "loginwindow.h"
#include "usermanagementwindow.h"
#include "topicmanagementwindow.h"
#include "topicbrowsingwindow.h"
#include "applicationreviewwindow.h"
#include "statisticswindow.h"
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(int userId, int userType, const QString& username, const QString& realName, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      userId(userId),
      userType(userType),
      username(username),
      realName(realName)
{
    ui->setupUi(this);
    initUI();
    initMenu();
    initStatusBar();
    initConnections();
    loadUserInterface();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    setWindowTitle(tr("大学生小组课题选题管理系统 - %1").arg(realName));
    setMinimumSize(1024, 768);
    resize(1200, 800);

    // 创建主堆叠窗口
    mainStackedWidget = new QStackedWidget(this);
    setCentralWidget(mainStackedWidget);

    // 设置样式
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f8f9fa;
        }
        QMenuBar {
            background-color: #343a40;
            color: white;
            font-size: 12px;
            font-weight: bold;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 8px 16px;
        }
        QMenuBar::item:selected {
            background-color: #495057;
        }
        QMenu {
            background-color: white;
            border: 1px solid #dee2e6;
            font-size: 12px;
        }
        QMenu::item {
            padding: 8px 24px;
        }
        QMenu::item:selected {
            background-color: #007bff;
            color: white;
        }
        QStatusBar {
            background-color: #e9ecef;
            font-size: 11px;
        }
        QPushButton {
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 12px;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #dee2e6;
            gridline-color: #dee2e6;
        }
        QHeaderView::section {
            background-color: #e9ecef;
            padding: 8px;
            border: 1px solid #dee2e6;
            font-weight: bold;
        }
    )");
}

void MainWindow::initMenu()
{
    QMenuBar* menuBar = this->menuBar();

    // 文件菜单
    QMenu* fileMenu = menuBar->addMenu(tr("文件(&F)"));
    QAction* homeAction = fileMenu->addAction(tr("主页(&H)"));
    homeAction->setShortcut(QKeySequence::MoveToStartOfDocument);
    QAction* logoutAction = fileMenu->addAction(tr("退出登录(&L)"));
    logoutAction->setShortcut(QKeySequence::Quit);
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(tr("退出系统(&E)"));
    exitAction->setShortcut(QKeySequence::Close);

    // 管理菜单（根据用户类型显示不同菜单项）
    QMenu* manageMenu = menuBar->addMenu(tr("管理(&M)"));
    
    if (userType == Admin) {
        QAction* userManageAction = manageMenu->addAction(tr("用户管理(&U)"));
        connect(userManageAction, &QAction::triggered, this, &MainWindow::onUserManagementClicked);
        
        QAction* topicManageAction = manageMenu->addAction(tr("课题管理(&T)"));
        connect(topicManageAction, &QAction::triggered, this, &MainWindow::onTopicManagementClicked);
        
        QAction* applyManageAction = manageMenu->addAction(tr("申请管理(&A)"));
        connect(applyManageAction, &QAction::triggered, this, &MainWindow::onApplyManagementClicked);
    } else if (userType == Teacher) {
        QAction* topicManageAction = manageMenu->addAction(tr("我的课题(&T)"));
        connect(topicManageAction, &QAction::triggered, this, &MainWindow::onTopicManagementClicked);
        
        QAction* applyManageAction = manageMenu->addAction(tr("申请审核(&A)"));
        connect(applyManageAction, &QAction::triggered, this, &MainWindow::onApplyManagementClicked);
    } else if (userType == StudentGroup) {
        QAction* topicManageAction = manageMenu->addAction(tr("课题浏览(&T)"));
        connect(topicManageAction, &QAction::triggered, this, &MainWindow::onTopicManagementClicked);
        
        QAction* applyManageAction = manageMenu->addAction(tr("我的申请(&A)"));
        connect(applyManageAction, &QAction::triggered, this, &MainWindow::onApplyManagementClicked);
    }

    // 统计菜单
    if (userType == Admin || userType == Teacher) {
        QMenu* statsMenu = menuBar->addMenu(tr("统计(&S)"));
        QAction* statsAction = statsMenu->addAction(tr("数据统计(&D)"));
        connect(statsAction, &QAction::triggered, this, &MainWindow::onStatisticsClicked);
    }

    // 帮助菜单
    QMenu* helpMenu = menuBar->addMenu(tr("帮助(&H)"));
    QAction* profileAction = helpMenu->addAction(tr("个人信息(&P)"));
    connect(profileAction, &QAction::triggered, this, &MainWindow::onProfileClicked);
    helpMenu->addSeparator();
    QAction* aboutAction = helpMenu->addAction(tr("关于(&A)"));
    connect(aboutAction, &QAction::triggered, []() {
        QMessageBox::about(nullptr, tr("关于系统"), 
                          tr("大学生小组课题选题管理系统 v1.0.0\n\n" 
                             "基于Qt 6.5.3开发的C/S架构管理系统\n" 
                             "© 2024 EducationTech"));
    });

    // 连接菜单动作
    connect(homeAction, &QAction::triggered, this, &MainWindow::onHomeClicked);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onLogoutClicked);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::initStatusBar()
{
    QStatusBar* statusBar = this->statusBar();
    
    // 连接状态标签
    statusConnectionLabel = new QLabel(this);
    statusConnectionLabel->setStyleSheet("color: #28a745; font-weight: bold;");
    statusConnectionLabel->setText(tr("已连接"));
    statusBar->addWidget(statusConnectionLabel);
    
    // 分隔符
    statusBar->addPermanentWidget(new QLabel(" | ", this));
    
    // 用户信息标签
    statusUserLabel = new QLabel(this);
    QString userTypeStr;
    switch (userType) {
    case Admin: userTypeStr = tr("管理员"); break;
    case Teacher: userTypeStr = tr("教师"); break;
    case StudentGroup: userTypeStr = tr("学生小组"); break;
    }
    statusUserLabel->setText(tr("用户：%1 (%2)").arg(realName).arg(userTypeStr));
    statusBar->addPermanentWidget(statusUserLabel);
    
    // 分隔符
    statusBar->addPermanentWidget(new QLabel(" | ", this));
    
    // 时间标签
    statusTimeLabel = new QLabel(this);
    statusTimeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    statusBar->addPermanentWidget(statusTimeLabel);
    
    // 定时更新时间
    QTimer* timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, [=]() {
        statusTimeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    });
    timeTimer->start(1000);
}

void MainWindow::initConnections()
{
    NetworkManager& networkManager = NetworkManager::getInstance();
    
    connect(&networkManager, &NetworkManager::connectionStateChanged,
            this, [=](NetworkManager::ConnectionState state) {
        switch (state) {
        case NetworkManager::Connected:
            statusConnectionLabel->setText(tr("已连接"));
            statusConnectionLabel->setStyleSheet("color: #28a745; font-weight: bold;");
            break;
        case NetworkManager::Connecting:
            statusConnectionLabel->setText(tr("连接中"));
            statusConnectionLabel->setStyleSheet("color: #ffc107; font-weight: bold;");
            break;
        case NetworkManager::Disconnected:
            statusConnectionLabel->setText(tr("已断开"));
            statusConnectionLabel->setStyleSheet("color: #dc3545; font-weight: bold;");
            break;
        }
    });
    
    connect(&networkManager, &NetworkManager::responseReceived,
            this, &MainWindow::onResponseReceived);
    
    connect(&networkManager, &NetworkManager::errorOccurred,
            this, [=](NetworkManager::ErrorType errorType, const QString& errorMsg) {
        QMessageBox::warning(this, tr("网络错误"), errorMsg);
    });
}

void MainWindow::loadUserInterface()
{
    switch (userType) {
    case Admin:
        createAdminInterface();
        break;
    case Teacher:
        createTeacherInterface();
        break;
    case StudentGroup:
        createStudentInterface();
        break;
    }
    
    showHomePage();
}

void MainWindow::createAdminInterface()
{
    // 创建管理员界面的各种页面
    showHomePage();
}

void MainWindow::createTeacherInterface()
{
    // 创建教师界面的各种页面
    showHomePage();
}

void MainWindow::createStudentInterface()
{
    // 创建学生界面的各种页面
    showHomePage();
}

void MainWindow::showHomePage()
{
    // 检查是否已经存在主页
    for (int i = 0; i < mainStackedWidget->count(); ++i) {
        if (mainStackedWidget->widget(i)->objectName() == "HomePage") {
            mainStackedWidget->setCurrentIndex(i);
            return;
        }
    }

    // 创建主页
    QWidget* homePage = new QWidget();
    homePage->setObjectName("HomePage");
    QVBoxLayout* layout = new QVBoxLayout(homePage);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    // 欢迎标题
    QLabel* welcomeLabel = new QLabel(tr("欢迎使用大学生小组课题选题管理系统！"), homePage);
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #343a40;");
    layout->addWidget(welcomeLabel, 0, Qt::AlignCenter);

    // 用户信息卡片
    QWidget* infoCard = new QWidget(homePage);
    infoCard->setStyleSheet("background-color: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 12px rgba(0, 0, 0, 0.1);");
    QVBoxLayout* cardLayout = new QVBoxLayout(infoCard);
    
    QLabel* userInfoLabel = new QLabel(tr("当前用户信息"), infoCard);
    userInfoLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #343a40; margin-bottom: 15px;");
    cardLayout->addWidget(userInfoLabel);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    
    QString userTypeStr;
    switch (userType) {
    case Admin: userTypeStr = tr("系统管理员"); break;
    case Teacher: userTypeStr = tr("指导教师"); break;
    case StudentGroup: userTypeStr = tr("学生小组"); break;
    }
    
    gridLayout->addWidget(new QLabel(tr("用户ID："), infoCard), 0, 0);
    gridLayout->addWidget(new QLabel(QString::number(userId), infoCard), 0, 1);
    gridLayout->addWidget(new QLabel(tr("用户名："), infoCard), 1, 0);
    gridLayout->addWidget(new QLabel(username, infoCard), 1, 1);
    gridLayout->addWidget(new QLabel(tr("真实姓名："), infoCard), 2, 0);
    gridLayout->addWidget(new QLabel(realName, infoCard), 2, 1);
    gridLayout->addWidget(new QLabel(tr("用户类型："), infoCard), 3, 0);
    gridLayout->addWidget(new QLabel(userTypeStr, infoCard), 3, 1);
    gridLayout->addWidget(new QLabel(tr("登录时间："), infoCard), 4, 0);
    gridLayout->addWidget(new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), infoCard), 4, 1);
    
    cardLayout->addLayout(gridLayout);
    layout->addWidget(infoCard);
    
    // 系统状态卡片
    QWidget* statusCard = new QWidget(homePage);
    statusCard->setStyleSheet("background-color: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 12px rgba(0, 0, 0, 0.1);");
    QVBoxLayout* statusLayout = new QVBoxLayout(statusCard);
    
    QLabel* statusLabel = new QLabel(tr("系统状态"), statusCard);
    statusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #343a40; margin-bottom: 15px;");
    statusLayout->addWidget(statusLabel);
    
    QHBoxLayout* statusRowLayout = new QHBoxLayout();
    statusRowLayout->setSpacing(20);
    
    // 连接状态
    QWidget* connStatusWidget = new QWidget(statusCard);
    QVBoxLayout* connLayout = new QVBoxLayout(connStatusWidget);
    connLayout->setAlignment(Qt::AlignCenter);
    QLabel* connIcon = new QLabel("🟢", connStatusWidget);
    connIcon->setStyleSheet("font-size: 32px;");
    QLabel* connText = new QLabel(tr("服务器已连接"), connStatusWidget);
    connText->setStyleSheet("font-size: 12px; color: #6c757d;");
    connLayout->addWidget(connIcon);
    connLayout->addWidget(connText);
    statusRowLayout->addWidget(connStatusWidget);
    
    // 系统版本
    QWidget* versionWidget = new QWidget(statusCard);
    QVBoxLayout* versionLayout = new QVBoxLayout(versionWidget);
    versionLayout->setAlignment(Qt::AlignCenter);
    QLabel* versionIcon = new QLabel("📊", versionWidget);
    versionIcon->setStyleSheet("font-size: 32px;");
    QLabel* versionText = new QLabel(tr("版本：1.0.0"), versionWidget);
    versionText->setStyleSheet("font-size: 12px; color: #6c757d;");
    versionLayout->addWidget(versionIcon);
    versionLayout->addWidget(versionText);
    statusRowLayout->addWidget(versionWidget);
    
    statusLayout->addLayout(statusRowLayout);
    layout->addWidget(statusCard);
    
    // 快速操作按钮
    QWidget* quickActionsWidget = new QWidget(homePage);
    QHBoxLayout* quickLayout = new QHBoxLayout(quickActionsWidget);
    quickLayout->setSpacing(20);
    quickLayout->setAlignment(Qt::AlignCenter);
    
    if (userType == Admin) {
        QPushButton* userBtn = new QPushButton(tr("用户管理"), homePage);
        userBtn->setFixedSize(120, 40);
        connect(userBtn, &QPushButton::clicked, this, &MainWindow::onUserManagementClicked);
        quickLayout->addWidget(userBtn);
        
        QPushButton* topicBtn = new QPushButton(tr("课题管理"), homePage);
        topicBtn->setFixedSize(120, 40);
        connect(topicBtn, &QPushButton::clicked, this, &MainWindow::onTopicManagementClicked);
        quickLayout->addWidget(topicBtn);
    }
    
    QPushButton* applyBtn = new QPushButton(
        userType == Admin ? tr("申请管理") : 
        userType == Teacher ? tr("申请审核") : tr("我的申请"), homePage);
    applyBtn->setFixedSize(120, 40);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::onApplyManagementClicked);
    quickLayout->addWidget(applyBtn);
    
    layout->addWidget(quickActionsWidget);
    
    layout->addStretch();
    
    mainStackedWidget->addWidget(homePage);
    mainStackedWidget->setCurrentWidget(homePage);
}

void MainWindow::showUserManagementPage()
{
    // 检查是否已经存在用户管理页面
    for (int i = 0; i < mainStackedWidget->count(); ++i) {
        if (mainStackedWidget->widget(i)->objectName() == "UserManagementPage") {
            mainStackedWidget->setCurrentIndex(i);
            return;
        }
    }

    // 创建用户管理页面
    UserManagementWindow* userPage = new UserManagementWindow();
    userPage->setObjectName("UserManagementPage");
    mainStackedWidget->addWidget(userPage);
    mainStackedWidget->setCurrentWidget(userPage);
}

void MainWindow::showTopicManagementPage()
{
    // 根据用户类型显示不同的页面
    if (userType == StudentGroup) {
        // 学生小组显示课题浏览页面
        for (int i = 0; i < mainStackedWidget->count(); ++i) {
            if (mainStackedWidget->widget(i)->objectName() == "TopicBrowsingPage") {
                mainStackedWidget->setCurrentIndex(i);
                return;
            }
        }

        // 创建课题浏览页面
        TopicBrowsingWindow* browsingPage = new TopicBrowsingWindow(userId, userType);
        browsingPage->setObjectName("TopicBrowsingPage");
        mainStackedWidget->addWidget(browsingPage);
        mainStackedWidget->setCurrentWidget(browsingPage);
    } else {
        // 管理员和教师显示课题管理页面
        for (int i = 0; i < mainStackedWidget->count(); ++i) {
            if (mainStackedWidget->widget(i)->objectName() == "TopicManagementPage") {
                mainStackedWidget->setCurrentIndex(i);
                return;
            }
        }

        // 创建课题管理页面
        TopicManagementWindow* topicPage = new TopicManagementWindow(userType, userId);
        topicPage->setObjectName("TopicManagementPage");
        mainStackedWidget->addWidget(topicPage);
        mainStackedWidget->setCurrentWidget(topicPage);
    }
}

void MainWindow::showApplicationReviewPage()
{
    // 检查是否已经存在申请审核页面
    for (int i = 0; i < mainStackedWidget->count(); ++i) {
        if (mainStackedWidget->widget(i)->objectName() == "ApplicationReviewPage") {
            mainStackedWidget->setCurrentIndex(i);
            return;
        }
    }

    // 创建申请审核页面
    ApplicationReviewWindow* reviewPage = new ApplicationReviewWindow(userType, userId);
    reviewPage->setObjectName("ApplicationReviewPage");
    mainStackedWidget->addWidget(reviewPage);
    mainStackedWidget->setCurrentWidget(reviewPage);
}

void MainWindow::showStatisticsPage()
{
    // 检查是否已经存在统计页面
    for (int i = 0; i < mainStackedWidget->count(); ++i) {
        if (mainStackedWidget->widget(i)->objectName() == "StatisticsPage") {
            mainStackedWidget->setCurrentIndex(i);
            return;
        }
    }

    // 创建统计页面
    StatisticsWindow* statsPage = new StatisticsWindow(userType, userId);
    statsPage->setObjectName("StatisticsPage");
    mainStackedWidget->addWidget(statsPage);
    mainStackedWidget->setCurrentWidget(statsPage);
}

void MainWindow::showProfilePage()
{
    QMessageBox::information(this, tr("功能提示"), tr("个人信息功能正在开发中..."));
}

void MainWindow::onHomeClicked()
{
    showHomePage();
}

void MainWindow::onUserManagementClicked()
{
    showUserManagementPage();
}

void MainWindow::onTopicManagementClicked()
{
    showTopicManagementPage();
}

void MainWindow::onApplyManagementClicked()
{
    NetworkManager::ConnectionState state = NetworkManager::getInstance().getConnectionState();
    if (state == NetworkManager::Disconnected) {
        QMessageBox::warning(this, tr("网络错误"), tr("当前未连接到服务器，请检查网络连接后重试。"));
        return;
    }
    showApplicationReviewPage();
}

void MainWindow::onStatisticsClicked()
{
    showStatisticsPage();
}

void MainWindow::onProfileClicked()
{
    showProfilePage();
}

void MainWindow::onLogoutClicked()
{
    int ret = QMessageBox::question(this, tr("确认退出登录"), 
                                   tr("确定要退出登录吗？"),
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 清除用户信息
        NetworkManager::getInstance().clearCurrentUser();
        
        // 关闭主窗口，返回登录界面
        this->close();
        
        // 创建并显示登录窗口
        LoginWindow* loginWindow = new LoginWindow();
        loginWindow->show();
    }
}

void MainWindow::onConnectionStateChanged(int state)
{
    updateStatusBar();
}

void MainWindow::onResponseReceived(const QString& requestId, const QJsonObject& response)
{
    // 处理服务器响应
    qDebug() << "Received response for request:" << requestId;
}

void MainWindow::updateStatusBar()
{
    // 更新状态栏信息
    NetworkManager::ConnectionState state = NetworkManager::getInstance().getConnectionState();
    
    switch (state) {
    case NetworkManager::Connected:
        statusConnectionLabel->setText(tr("已连接"));
        statusConnectionLabel->setStyleSheet("color: #28a745; font-weight: bold;");
        break;
    case NetworkManager::Connecting:
        statusConnectionLabel->setText(tr("连接中"));
        statusConnectionLabel->setStyleSheet("color: #ffc107; font-weight: bold;");
        break;
    case NetworkManager::Disconnected:
        statusConnectionLabel->setText(tr("已断开"));
        statusConnectionLabel->setStyleSheet("color: #dc3545; font-weight: bold;");
        break;
    }
}
