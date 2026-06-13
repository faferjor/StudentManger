#include "usermanagementwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>
#include <QLabel>
#include <QGroupBox>
#include "networkmanager.h"

UserManagementWindow::UserManagementWindow(QWidget *parent) : QWidget(parent),
    editingUserId(0)
{
    initUI();
    initConnections();
    loadUsers();
}

UserManagementWindow::~UserManagementWindow()
{
    if (userDialog) {
        delete userDialog;
    }
}

void UserManagementWindow::initUI()
{
    setWindowTitle(tr("用户管理"));
    setMinimumSize(1000, 600);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    QLabel* titleLabel = new QLabel(tr("用户管理"), this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #343a40;");
    mainLayout->addWidget(titleLabel);

    // 搜索和筛选区域
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(10);

    QLabel* searchLabel = new QLabel(tr("搜索:"), this);
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText(tr("输入用户名或真实姓名"));
    searchLineEdit->setMinimumWidth(200);

    QLabel* filterLabel = new QLabel(tr("用户类型:"), this);
    userTypeFilterComboBox = new QComboBox(this);
    userTypeFilterComboBox->addItem(tr("全部"), -1);
    userTypeFilterComboBox->addItem(tr("管理员"), 1);
    userTypeFilterComboBox->addItem(tr("教师"), 2);
    userTypeFilterComboBox->addItem(tr("学生小组"), 3);

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(filterLabel);
    searchLayout->addWidget(userTypeFilterComboBox);
    searchLayout->addStretch();

    mainLayout->addLayout(searchLayout);

    // 用户表格
    userTable = new QTableWidget(this);
    userTable->setColumnCount(8);
    QStringList headers = {tr("用户ID"), tr("账号"), tr("真实姓名"), tr("用户类型"), tr("邮箱"), tr("电话"), tr("状态"), tr("创建时间")};
    userTable->setHorizontalHeaderLabels(headers);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    userTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    userTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    userTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    mainLayout->addWidget(userTable);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    addUserButton = new QPushButton(tr("添加用户"), this);
    addUserButton->setStyleSheet("background-color: #28a745; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    
    editUserButton = new QPushButton(tr("编辑用户"), this);
    editUserButton->setStyleSheet("background-color: #007bff; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    editUserButton->setEnabled(false);
    
    deleteUserButton = new QPushButton(tr("删除用户"), this);
    deleteUserButton->setStyleSheet("background-color: #dc3545; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    deleteUserButton->setEnabled(false);
    
    refreshButton = new QPushButton(tr("刷新"), this);
    refreshButton->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 16px; border: none; border-radius: 4px;");

    buttonLayout->addWidget(addUserButton);
    buttonLayout->addWidget(editUserButton);
    buttonLayout->addWidget(deleteUserButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void UserManagementWindow::initConnections()
{
    connect(addUserButton, &QPushButton::clicked, this, &UserManagementWindow::onAddUserClicked);
    connect(editUserButton, &QPushButton::clicked, this, &UserManagementWindow::onEditUserClicked);
    connect(deleteUserButton, &QPushButton::clicked, this, &UserManagementWindow::onDeleteUserClicked);
    connect(refreshButton, &QPushButton::clicked, this, &UserManagementWindow::onRefreshClicked);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &UserManagementWindow::onSearchTextChanged);
    connect(userTypeFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UserManagementWindow::onUserTypeFilterChanged);
    connect(userTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !userTable->selectedItems().isEmpty();
        editUserButton->setEnabled(hasSelection);
        deleteUserButton->setEnabled(hasSelection);
    });
    connect(&NetworkManager::getInstance(), &NetworkManager::responseReceived, this, &UserManagementWindow::onUserResponseReceived);
}

void UserManagementWindow::loadUsers()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    int userType = userTypeFilterComboBox->currentData().toInt();
    QJsonObject params;
    params["user_type"] = userType;
    currentRequestType = "GET_USERS";  // 保存请求类型
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_USERS", params);
}

void UserManagementWindow::updateUserTable(const QJsonArray& users)
{
    qDebug() << "=== updateUserTable called, users size:" << users.size();
    qDebug() << "userTable is null:" << (userTable == nullptr);
    
    userTable->setRowCount(0);
    qDebug() << "Row count set to 0";

    for (const QJsonValue& value : users) {
        QJsonObject user = value.toObject();
        int row = userTable->rowCount();
        userTable->insertRow(row);

        int userId = user.value("user_id").toInt();
        QString username = user.value("username").toString();
        QString realName = user.value("real_name").toString();
        int userType = user.value("user_type").toInt();
        QString email = user.value("email").toString();
        QString phone = user.value("phone").toString();
        int status = user.value("status").toInt();
        QString createTime = user.value("create_time").toString();

        QString userTypeStr;
        switch (userType) {
        case 1: userTypeStr = tr("管理员"); break;
        case 2: userTypeStr = tr("教师"); break;
        case 3: userTypeStr = tr("学生小组"); break;
        default: userTypeStr = tr("未知"); break;
        }

        QString statusStr = status == 1 ? tr("正常") : tr("禁用");

        userTable->setItem(row, 0, new QTableWidgetItem(QString::number(userId)));
        userTable->setItem(row, 1, new QTableWidgetItem(username));
        userTable->setItem(row, 2, new QTableWidgetItem(realName));
        userTable->setItem(row, 3, new QTableWidgetItem(userTypeStr));
        userTable->setItem(row, 4, new QTableWidgetItem(email));
        userTable->setItem(row, 5, new QTableWidgetItem(phone));
        userTable->setItem(row, 6, new QTableWidgetItem(statusStr));
        userTable->setItem(row, 7, new QTableWidgetItem(createTime));

        qDebug() << "Added row" << row << "for user:" << username;

        // 存储用户数据到行的隐藏数据
        QTableWidgetItem* idItem = userTable->item(row, 0);
        idItem->setData(Qt::UserRole, QVariant::fromValue(user));
    }
    
    qDebug() << "=== updateUserTable finished, total rows:" << userTable->rowCount();
}

void UserManagementWindow::showUserDialog(const QJsonObject& userInfo)
{
    bool isEdit = !userInfo.isEmpty();
    userDialog = new QDialog(this);
    userDialog->setWindowTitle(isEdit ? tr("编辑用户") : tr("添加用户"));
    userDialog->setMinimumWidth(500);
    userDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(userDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    // 基本信息表单
    QGroupBox* basicGroup = new QGroupBox(tr("基本信息"), userDialog);
    QFormLayout* basicLayout = new QFormLayout(basicGroup);
    basicLayout->setSpacing(10);

    usernameLineEdit = new QLineEdit(userDialog);
    usernameLineEdit->setPlaceholderText(tr("请输入账号"));
    usernameLineEdit->setReadOnly(isEdit);
    if (isEdit) {
        usernameLineEdit->setText(userInfo.value("username").toString());
    }

    passwordLineEdit = new QLineEdit(userDialog);
    passwordLineEdit->setPlaceholderText(isEdit ? tr("留空表示不修改密码") : tr("请输入密码"));
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    if (!isEdit) {
        passwordLineEdit->setMinimumWidth(150);
    }

    realNameLineEdit = new QLineEdit(userDialog);
    realNameLineEdit->setPlaceholderText(tr("请输入真实姓名/小组名称"));
    if (isEdit) {
        realNameLineEdit->setText(userInfo.value("real_name").toString());
    }

    userTypeComboBox = new QComboBox(userDialog);
    userTypeComboBox->addItem(tr("管理员"), 1);
    userTypeComboBox->addItem(tr("教师"), 2);
    userTypeComboBox->addItem(tr("学生小组"), 3);
    if (isEdit) {
        userTypeComboBox->setCurrentIndex(userTypeComboBox->findData(userInfo.value("user_type").toInt()));
        userTypeComboBox->setEnabled(false);
    }

    statusComboBox = new QComboBox(userDialog);
    statusComboBox->addItem(tr("正常"), 1);
    statusComboBox->addItem(tr("禁用"), 0);
    if (isEdit) {
        statusComboBox->setCurrentIndex(statusComboBox->findData(userInfo.value("status").toInt()));
    } else {
        statusComboBox->setCurrentIndex(0);
    }

    emailLineEdit = new QLineEdit(userDialog);
    emailLineEdit->setPlaceholderText(tr("请输入邮箱（可选）"));
    if (isEdit) {
        emailLineEdit->setText(userInfo.value("email").toString());
    }

    phoneLineEdit = new QLineEdit(userDialog);
    phoneLineEdit->setPlaceholderText(tr("请输入电话（可选）"));
    if (isEdit) {
        phoneLineEdit->setText(userInfo.value("phone").toString());
    }

    basicLayout->addRow(tr("账号:"), usernameLineEdit);
    basicLayout->addRow(isEdit ? tr("新密码:") : tr("密码:"), passwordLineEdit);
    basicLayout->addRow(tr("真实姓名/小组名称:"), realNameLineEdit);
    basicLayout->addRow(tr("用户类型:"), userTypeComboBox);
    basicLayout->addRow(tr("状态:"), statusComboBox);
    basicLayout->addRow(tr("邮箱:"), emailLineEdit);
    basicLayout->addRow(tr("电话:"), phoneLineEdit);

    dialogLayout->addWidget(basicGroup);

    // 扩展信息表单
    extendGroup = new QGroupBox(userDialog);
    QFormLayout* extendLayout = new QFormLayout(extendGroup);
    extendLayout->setSpacing(10);

    int userType = isEdit ? userInfo.value("user_type").toInt() : userTypeComboBox->currentData().toInt();
    
    if (userType == 2) { // 教师
        extendGroup->setTitle(tr("教师信息"));
        
        departmentLineEdit = new QLineEdit(userDialog);
        departmentLineEdit->setPlaceholderText(tr("请输入所属院系"));
        if (isEdit) {
            departmentLineEdit->setText(userInfo.value("department").toString());
        }

        titleLineEdit = new QLineEdit(userDialog);
        titleLineEdit->setPlaceholderText(tr("请输入职称"));
        if (isEdit) {
            titleLineEdit->setText(userInfo.value("title").toString());
        }

        researchAreaTextEdit = new QTextEdit(userDialog);
        researchAreaTextEdit->setPlaceholderText(tr("请输入研究方向"));
        researchAreaTextEdit->setMaximumHeight(80);
        if (isEdit) {
            researchAreaTextEdit->setText(userInfo.value("research_area").toString());
        }

        extendLayout->addRow(tr("所属院系:"), departmentLineEdit);
        extendLayout->addRow(tr("职称:"), titleLineEdit);
        extendLayout->addRow(tr("研究方向:"), researchAreaTextEdit);
    } else if (userType == 3) { // 学生小组
        extendGroup->setTitle(tr("小组信息"));
        
        leaderNameLineEdit = new QLineEdit(userDialog);
        leaderNameLineEdit->setPlaceholderText(tr("请输入组长姓名"));
        if (isEdit) {
            leaderNameLineEdit->setText(userInfo.value("leader_name").toString());
        }

        memberCountLineEdit = new QLineEdit(userDialog);
        memberCountLineEdit->setPlaceholderText(tr("请输入成员人数"));
        memberCountLineEdit->setValidator(new QIntValidator(1, 20, this));
        if (isEdit) {
            memberCountLineEdit->setText(QString::number(userInfo.value("member_count").toInt()));
        } else {
            memberCountLineEdit->setText("1");
        }

        membersTextEdit = new QTextEdit(userDialog);
        membersTextEdit->setPlaceholderText(tr("请输入成员列表（每行一个）"));
        membersTextEdit->setMaximumHeight(80);
        if (isEdit) {
            membersTextEdit->setText(userInfo.value("members").toString());
        }

        majorLineEdit = new QLineEdit(userDialog);
        majorLineEdit->setPlaceholderText(tr("请输入专业"));
        if (isEdit) {
            majorLineEdit->setText(userInfo.value("major").toString());
        }

        gradeLineEdit = new QLineEdit(userDialog);
        gradeLineEdit->setPlaceholderText(tr("请输入年级"));
        if (isEdit) {
            gradeLineEdit->setText(userInfo.value("grade").toString());
        }

        classNameLineEdit = new QLineEdit(userDialog);
        classNameLineEdit->setPlaceholderText(tr("请输入班级"));
        if (isEdit) {
            classNameLineEdit->setText(userInfo.value("class_name").toString());
        }

        extendLayout->addRow(tr("组长姓名:"), leaderNameLineEdit);
        extendLayout->addRow(tr("成员人数:"), memberCountLineEdit);
        extendLayout->addRow(tr("成员列表:"), membersTextEdit);
        extendLayout->addRow(tr("专业:"), majorLineEdit);
        extendLayout->addRow(tr("年级:"), gradeLineEdit);
        extendLayout->addRow(tr("班级:"), classNameLineEdit);
    }

    if (userType == 2 || userType == 3) {
        dialogLayout->addWidget(extendGroup);
    }

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addStretch();

    QPushButton* cancelButton = new QPushButton(tr("取消"), userDialog);
    connect(cancelButton, &QPushButton::clicked, userDialog, &QDialog::reject);

    QPushButton* okButton = new QPushButton(isEdit ? tr("保存修改") : tr("添加用户"), userDialog);
    okButton->setStyleSheet("background-color: #007bff; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    connect(okButton, &QPushButton::clicked, this, isEdit ? &UserManagementWindow::onEditUserDialogAccepted : &UserManagementWindow::onAddUserDialogAccepted);

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    dialogLayout->addLayout(buttonLayout);

    // 连接用户类型变化信号 - 动态更新扩展信息区域
    if (!isEdit) {
        connect(userTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            Q_UNUSED(index);
            // 先移除旧的扩展组
            if (dialogLayout->indexOf(extendGroup) != -1) {
                dialogLayout->removeWidget(extendGroup);
                extendGroup->deleteLater();
                extendGroup = nullptr;
            }
            
            // 获取新的用户类型
            int newUserType = userTypeComboBox->currentData().toInt();
            
            // 重新创建扩展信息表单
            extendGroup = new QGroupBox(userDialog);
            QFormLayout* extendLayout = new QFormLayout(extendGroup);
            extendLayout->setSpacing(10);
            
            if (newUserType == 2) { // 教师
                extendGroup->setTitle(tr("教师信息"));
                
                departmentLineEdit = new QLineEdit(userDialog);
                departmentLineEdit->setPlaceholderText(tr("请输入所属院系"));
                
                titleLineEdit = new QLineEdit(userDialog);
                titleLineEdit->setPlaceholderText(tr("请输入职称"));
                
                researchAreaTextEdit = new QTextEdit(userDialog);
                researchAreaTextEdit->setPlaceholderText(tr("请输入研究方向"));
                researchAreaTextEdit->setMaximumHeight(80);
                
                extendLayout->addRow(tr("所属院系:"), departmentLineEdit);
                extendLayout->addRow(tr("职称:"), titleLineEdit);
                extendLayout->addRow(tr("研究方向:"), researchAreaTextEdit);
            } else if (newUserType == 3) { // 学生小组
                extendGroup->setTitle(tr("小组信息"));
                
                leaderNameLineEdit = new QLineEdit(userDialog);
                leaderNameLineEdit->setPlaceholderText(tr("请输入组长姓名"));
                
                memberCountLineEdit = new QLineEdit(userDialog);
                memberCountLineEdit->setPlaceholderText(tr("请输入成员人数"));
                memberCountLineEdit->setValidator(new QIntValidator(1, 20, this));
                memberCountLineEdit->setText("1");
                
                membersTextEdit = new QTextEdit(userDialog);
                membersTextEdit->setPlaceholderText(tr("请输入成员列表（每行一个）"));
                membersTextEdit->setMaximumHeight(80);
                
                majorLineEdit = new QLineEdit(userDialog);
                majorLineEdit->setPlaceholderText(tr("请输入专业"));
                
                gradeLineEdit = new QLineEdit(userDialog);
                gradeLineEdit->setPlaceholderText(tr("请输入年级"));
                
                classNameLineEdit = new QLineEdit(userDialog);
                classNameLineEdit->setPlaceholderText(tr("请输入班级"));
                
                extendLayout->addRow(tr("组长姓名:"), leaderNameLineEdit);
                extendLayout->addRow(tr("成员人数:"), memberCountLineEdit);
                extendLayout->addRow(tr("成员列表:"), membersTextEdit);
                extendLayout->addRow(tr("专业:"), majorLineEdit);
                extendLayout->addRow(tr("年级:"), gradeLineEdit);
                extendLayout->addRow(tr("班级:"), classNameLineEdit);
            }
            
            // 添加新的扩展组到布局中（在按钮组之前）
            if (newUserType == 2 || newUserType == 3) {
                dialogLayout->insertWidget(dialogLayout->count() - 1, extendGroup);
            }
            
            // 调整对话框大小
            userDialog->adjustSize();
        });
    }

    userDialog->exec();
}

void UserManagementWindow::onAddUserClicked()
{
    showUserDialog();
}

void UserManagementWindow::onEditUserClicked()
{
    QModelIndexList selected = userTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    QTableWidgetItem* idItem = userTable->item(row, 0);
    QJsonObject userInfo = idItem->data(Qt::UserRole).toJsonObject();

    showUserDialog(userInfo);
}

void UserManagementWindow::onDeleteUserClicked()
{
    QModelIndexList selected = userTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int userId = userTable->item(row, 0)->text().toInt();
    QString username = userTable->item(row, 1)->text();

    int ret = QMessageBox::question(this, tr("确认删除"),
                                   tr("确定要删除用户 '%1' 吗？\n此操作不可恢复。").arg(username),
                                   QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        QJsonObject params;
        params["user_id"] = userId;
        if (!currentRequestId.isEmpty()) {
            NetworkManager::getInstance().cancelRequest(currentRequestId);
            currentRequestId.clear();
        }
        currentRequestType = "DELETE_USER";  // 保存请求类型
        currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("DELETE_USER", params);
    }
}

void UserManagementWindow::onRefreshClicked()
{
    loadUsers();
}

void UserManagementWindow::onSearchTextChanged(const QString& text)
{
    // 简单的客户端搜索过滤
    for (int i = 0; i < userTable->rowCount(); ++i) {
        QString username = userTable->item(i, 1)->text();
        QString realName = userTable->item(i, 2)->text();
        bool match = username.contains(text, Qt::CaseInsensitive) || realName.contains(text, Qt::CaseInsensitive);
        userTable->setRowHidden(i, !match);
    }
}

void UserManagementWindow::onUserTypeFilterChanged(int index)
{
    Q_UNUSED(index);
    loadUsers();
}

void UserManagementWindow::onUserResponseReceived(const QString& requestId, const QJsonObject& response)
{
    qDebug() << "=== onUserResponseReceived called ===";
    qDebug() << "Received requestId:" << requestId;
    qDebug() << "Current requestId:" << currentRequestId;
    qDebug() << "Current requestType:" << currentRequestType;
    qDebug() << "Response:" << QJsonDocument(response).toJson(QJsonDocument::Compact);

    if (requestId != currentRequestId) {
        qDebug() << "RequestId doesn't match, ignoring";
        return;
    }

    QString status = response.value("status").toString();
    qDebug() << "Status:" << status;
    
    if (status == "success") {
        QJsonObject data = response.value("data").toObject();
        qDebug() << "Data keys:" << data.keys();
        
        if (currentRequestType == "GET_USERS") {
            qDebug() << "Processing GET_USERS";
            QJsonArray users = data.value("users").toArray();
            qDebug() << "Updating user table with" << users.size() << "users";
            updateUserTable(users);
            // 移除这个弹窗，避免干扰
            // QMessageBox::information(this, tr("成功"), tr("用户数据加载成功"));
        } else if (currentRequestType == "ADD_USER") {
            qDebug() << "Processing ADD_USER";
            // 移除弹窗，直接关闭对话框并刷新
            if (userDialog) {
                userDialog->accept();
            }
            // 先清空，再刷新
            currentRequestId.clear();
            currentRequestType.clear();
            loadUsers();
            return;  // 直接返回，避免后面再次清空
        } else if (currentRequestType == "UPDATE_USER") {
            qDebug() << "Processing UPDATE_USER";
            // 移除弹窗，直接关闭对话框并刷新
            if (userDialog) {
                userDialog->accept();
            }
            // 先清空，再刷新
            currentRequestId.clear();
            currentRequestType.clear();
            loadUsers();
            return;  // 直接返回，避免后面再次清空
        } else if (currentRequestType == "DELETE_USER") {
            qDebug() << "Processing DELETE_USER";
            // 先清空，再刷新
            currentRequestId.clear();
            currentRequestType.clear();
            // 移除弹窗，直接刷新
            loadUsers();
            return;  // 直接返回，避免后面再次清空
        } else {
            qDebug() << "Unknown request type:" << currentRequestType;
        }
    } else {
        qDebug() << "Request failed";
        QString message = response.value("message").toString();
        QMessageBox::warning(this, tr("失败"), message.isEmpty() ? tr("操作失败，请重试") : message);
    }

    qDebug() << "Clearing currentRequestId and currentRequestType";
    currentRequestId.clear();
    currentRequestType.clear();
}

void UserManagementWindow::onAddUserDialogAccepted()
{
    QString username = usernameLineEdit->text().trimmed();
    QString password = passwordLineEdit->text().trimmed();
    QString realName = realNameLineEdit->text().trimmed();
    int userType = userTypeComboBox->currentData().toInt();
    int status = statusComboBox->currentData().toInt();
    QString email = emailLineEdit->text().trimmed();
    QString phone = phoneLineEdit->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(userDialog, tr("输入错误"), tr("账号不能为空"));
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(userDialog, tr("输入错误"), tr("密码不能为空"));
        return;
    }

    if (password.length() < 6) {
        QMessageBox::warning(userDialog, tr("输入错误"), tr("密码长度不能少于6位"));
        return;
    }

    if (realName.isEmpty()) {
        QMessageBox::warning(userDialog, tr("输入错误"), tr("真实姓名不能为空"));
        return;
    }

    QJsonObject userInfo;
    userInfo["username"] = username;
    userInfo["password"] = password;
    userInfo["real_name"] = realName;
    userInfo["user_type"] = userType;
    userInfo["status"] = status;
    userInfo["email"] = email;
    userInfo["phone"] = phone;

    if (userType == 2) { // 教师
        userInfo["department"] = departmentLineEdit->text().trimmed();
        userInfo["title"] = titleLineEdit->text().trimmed();
        userInfo["research_area"] = researchAreaTextEdit->toPlainText().trimmed();
    } else if (userType == 3) { // 学生小组
        userInfo["leader_name"] = leaderNameLineEdit->text().trimmed();
        userInfo["member_count"] = memberCountLineEdit->text().toInt();
        userInfo["members"] = membersTextEdit->toPlainText().trimmed();
        userInfo["major"] = majorLineEdit->text().trimmed();
        userInfo["grade"] = gradeLineEdit->text().trimmed();
        userInfo["class_name"] = classNameLineEdit->text().trimmed();
    }

    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    currentRequestType = "ADD_USER";  // 保存请求类型
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("ADD_USER", userInfo);
}

void UserManagementWindow::onEditUserDialogAccepted()
{
    QString username = usernameLineEdit->text().trimmed();
    QString password = passwordLineEdit->text().trimmed();
    QString realName = realNameLineEdit->text().trimmed();
    int userType = userTypeComboBox->currentData().toInt();
    int status = statusComboBox->currentData().toInt();
    QString email = emailLineEdit->text().trimmed();
    QString phone = phoneLineEdit->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(userDialog, tr("输入错误"), tr("账号不能为空"));
        return;
    }

    if (realName.isEmpty()) {
        QMessageBox::warning(userDialog, tr("输入错误"), tr("真实姓名不能为空"));
        return;
    }

    QJsonObject userInfo;
    userInfo["username"] = username;
    userInfo["real_name"] = realName;
    userInfo["user_type"] = userType;
    userInfo["status"] = status;
    userInfo["email"] = email;
    userInfo["phone"] = phone;

    if (!password.isEmpty()) {
        if (password.length() < 6) {
            QMessageBox::warning(userDialog, tr("输入错误"), tr("密码长度不能少于6位"));
            return;
        }
        userInfo["password"] = password;
    }

    if (userType == 2) { // 教师
        userInfo["department"] = departmentLineEdit->text().trimmed();
        userInfo["title"] = titleLineEdit->text().trimmed();
        userInfo["research_area"] = researchAreaTextEdit->toPlainText().trimmed();
    } else if (userType == 3) { // 学生小组
        userInfo["leader_name"] = leaderNameLineEdit->text().trimmed();
        userInfo["member_count"] = memberCountLineEdit->text().toInt();
        userInfo["members"] = membersTextEdit->toPlainText().trimmed();
        userInfo["major"] = majorLineEdit->text().trimmed();
        userInfo["grade"] = gradeLineEdit->text().trimmed();
        userInfo["class_name"] = classNameLineEdit->text().trimmed();
    }

    // 获取当前编辑的用户ID
    QModelIndexList selected = userTable->selectionModel()->selectedRows();
    if (!selected.isEmpty()) {
        int row = selected.first().row();
        int userId = userTable->item(row, 0)->text().toInt();
        userInfo["user_id"] = userId;
    }

    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    currentRequestType = "UPDATE_USER";  // 保存请求类型
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("UPDATE_USER", userInfo);
}
