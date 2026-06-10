#include "topicmanagementwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>
#include <QLabel>
#include <QGroupBox>
#include <QIntValidator>
#include "networkmanager.h"

TopicManagementWindow::TopicManagementWindow(int userType, int userId, QWidget *parent) : QWidget(parent),
    userType(userType),
    userId(userId)
{
    initUI();
    initConnections();
    loadTeachers();
    loadTopics();
}

TopicManagementWindow::~TopicManagementWindow()
{
    if (topicDialog) {
        delete topicDialog;
    }
}

void TopicManagementWindow::initUI()
{
    setWindowTitle(userType == 1 ? tr("课题管理") : tr("我的课题"));
    setMinimumSize(1000, 600);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    QLabel* titleLabel = new QLabel(userType == 1 ? tr("课题管理") : tr("我的课题"), this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #343a40;");
    mainLayout->addWidget(titleLabel);

    // 搜索和筛选区域
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(10);

    QLabel* searchLabel = new QLabel(tr("搜索:"), this);
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText(tr("输入课题名称"));
    searchLineEdit->setMinimumWidth(200);

    QLabel* statusLabel = new QLabel(tr("状态:"), this);
    statusFilterComboBox = new QComboBox(this);
    statusFilterComboBox->addItem(tr("全部"), -1);
    statusFilterComboBox->addItem(tr("可选题"), 1);
    statusFilterComboBox->addItem(tr("已关闭"), 0);

    if (userType == 1) { // 管理员
        QLabel* teacherLabel = new QLabel(tr("指导教师:"), this);
        teacherComboBox = new QComboBox(this);
        teacherComboBox->addItem(tr("全部教师"), -1);

        searchLayout->addWidget(searchLabel);
        searchLayout->addWidget(searchLineEdit);
        searchLayout->addWidget(statusLabel);
        searchLayout->addWidget(statusFilterComboBox);
        searchLayout->addWidget(teacherLabel);
        searchLayout->addWidget(teacherComboBox);
    } else {
        searchLayout->addWidget(searchLabel);
        searchLayout->addWidget(searchLineEdit);
        searchLayout->addWidget(statusLabel);
        searchLayout->addWidget(statusFilterComboBox);
    }

    searchLayout->addStretch();
    mainLayout->addLayout(searchLayout);

    // 课题表格
    topicTable = new QTableWidget(this);
    int columnCount = userType == 1 ? 8 : 7;
    topicTable->setColumnCount(columnCount);
    
    QStringList headers;
    if (userType == 1) {
        headers = {tr("课题ID"), tr("课题名称"), tr("指导教师"), tr("当前人数"), tr("最大人数"), tr("状态"), tr("创建时间"), tr("更新时间")};
    } else {
        headers = {tr("课题ID"), tr("课题名称"), tr("当前人数"), tr("最大人数"), tr("状态"), tr("创建时间"), tr("更新时间")};
    }
    
    topicTable->setHorizontalHeaderLabels(headers);
    topicTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    topicTable->setSelectionMode(QAbstractItemView::SingleSelection);
    topicTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    topicTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    topicTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    mainLayout->addWidget(topicTable);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    if (userType == 1 || userType == 2) { // 管理员和教师可以添加课题
        addTopicButton = new QPushButton(tr("添加课题"), this);
        addTopicButton->setStyleSheet("background-color: #28a745; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
        buttonLayout->addWidget(addTopicButton);
    }

    editTopicButton = new QPushButton(tr("编辑课题"), this);
    editTopicButton->setStyleSheet("background-color: #007bff; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    editTopicButton->setEnabled(false);
    
    deleteTopicButton = new QPushButton(tr("删除课题"), this);
    deleteTopicButton->setStyleSheet("background-color: #dc3545; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    deleteTopicButton->setEnabled(false);
    
    refreshButton = new QPushButton(tr("刷新"), this);
    refreshButton->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 16px; border: none; border-radius: 4px;");

    buttonLayout->addWidget(editTopicButton);
    buttonLayout->addWidget(deleteTopicButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void TopicManagementWindow::initConnections()
{
    if (addTopicButton) {
        connect(addTopicButton, &QPushButton::clicked, this, &TopicManagementWindow::onAddTopicClicked);
    }
    connect(editTopicButton, &QPushButton::clicked, this, &TopicManagementWindow::onEditTopicClicked);
    connect(deleteTopicButton, &QPushButton::clicked, this, &TopicManagementWindow::onDeleteTopicClicked);
    connect(refreshButton, &QPushButton::clicked, this, &TopicManagementWindow::onRefreshClicked);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &TopicManagementWindow::onSearchTextChanged);
    connect(statusFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TopicManagementWindow::onStatusFilterChanged);
    if (teacherComboBox) {
        connect(teacherComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TopicManagementWindow::onRefreshClicked);
    }
    connect(topicTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !topicTable->selectedItems().isEmpty();
        editTopicButton->setEnabled(hasSelection);
        deleteTopicButton->setEnabled(hasSelection);
    });
    connect(&NetworkManager::getInstance(), &NetworkManager::responseReceived, this, &TopicManagementWindow::onTopicResponseReceived);
}

void TopicManagementWindow::loadTeachers()
{
    if (userType != 1) { // 只有管理员需要加载教师列表
        return;
    }

    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    QJsonObject params;
    params["user_type"] = 2;
    currentRequestType = "GET_USERS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_USERS", params);
}

void TopicManagementWindow::loadTopics()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    int status = statusFilterComboBox->currentData().toInt();
    int teacherId = -1;
    
    if (userType == 1) { // 管理员
        teacherId = teacherComboBox->currentData().toInt();
    } else if (userType == 2) { // 教师
        // 需要先获取教师ID
        QJsonObject params;
        params["user_id"] = userId;
        currentRequestType = "GET_USER_INFO";
        currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_USER_INFO", params);
        return;
    }

    QJsonObject params;
    params["status"] = status;
    if (teacherId != -1) {
        params["teacher_id"] = teacherId;
    }

    currentRequestType = "GET_TOPICS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_TOPICS", params);
}

void TopicManagementWindow::updateTopicTable(const QJsonArray& topics)
{
    topicTable->setRowCount(0);

    for (const QJsonValue& value : topics) {
        QJsonObject topic = value.toObject();
        int row = topicTable->rowCount();
        topicTable->insertRow(row);

        int topicId = topic.value("topic_id").toInt();
        QString topicName = topic.value("topic_name").toString();
        QString teacherName = topic.value("teacher_name").toString();
        int currentStudents = topic.value("current_students").toInt();
        int maxStudents = topic.value("max_students").toInt();
        int status = topic.value("status").toInt();
        QString createTime = topic.value("create_time").toString();
        QString updateTime = topic.value("update_time").toString();

        QString statusStr = status == 1 ? tr("可选题") : tr("已关闭");

        int column = 0;
        topicTable->setItem(row, column++, new QTableWidgetItem(QString::number(topicId)));
        topicTable->setItem(row, column++, new QTableWidgetItem(topicName));
        
        if (userType == 1) { // 管理员显示指导教师
            topicTable->setItem(row, column++, new QTableWidgetItem(teacherName));
        }
        
        topicTable->setItem(row, column++, new QTableWidgetItem(QString::number(currentStudents)));
        topicTable->setItem(row, column++, new QTableWidgetItem(QString::number(maxStudents)));
        topicTable->setItem(row, column++, new QTableWidgetItem(statusStr));
        topicTable->setItem(row, column++, new QTableWidgetItem(createTime));
        topicTable->setItem(row, column++, new QTableWidgetItem(updateTime));

        // 存储课题数据到行的隐藏数据
        QTableWidgetItem* idItem = topicTable->item(row, 0);
        idItem->setData(Qt::UserRole, QVariant::fromValue(topic));
    }
}

void TopicManagementWindow::showTopicDialog(const QJsonObject& topicInfo)
{
    bool isEdit = !topicInfo.isEmpty();
    topicDialog = new QDialog(this);
    topicDialog->setWindowTitle(isEdit ? tr("编辑课题") : tr("添加课题"));
    topicDialog->setMinimumWidth(600);
    topicDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(topicDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    // 课题信息表单
    QGroupBox* topicGroup = new QGroupBox(tr("课题信息"), topicDialog);
    QFormLayout* topicLayout = new QFormLayout(topicGroup);
    topicLayout->setSpacing(10);

    topicNameLineEdit = new QLineEdit(topicDialog);
    topicNameLineEdit->setPlaceholderText(tr("请输入课题名称"));
    if (isEdit) {
        topicNameLineEdit->setText(topicInfo.value("topic_name").toString());
    }

    topicTeacherComboBox = new QComboBox(topicDialog);
    if (userType == 1) { // 管理员
        topicTeacherComboBox->addItem(tr("请选择指导教师"), 0);
        for (const QJsonValue& value : teachers) {
            QJsonObject teacher = value.toObject();
            int teacherId = teacher.value("user_id").toInt();
            QString teacherName = teacher.value("real_name").toString();
            topicTeacherComboBox->addItem(teacherName, teacherId);
        }
        if (isEdit) {
            int teacherId = topicInfo.value("teacher_id").toInt();
            int index = topicTeacherComboBox->findData(teacherId);
            if (index != -1) {
                topicTeacherComboBox->setCurrentIndex(index);
            }
        }
    } else if (userType == 2) { // 教师
        // 教师只能选择自己作为指导教师
        QJsonObject userInfo = NetworkManager::getInstance().getCurrentUser();
        QString realName = userInfo.value("real_name").toString();
        topicTeacherComboBox->addItem(realName, userId);
        topicTeacherComboBox->setEnabled(false);
    }

    descriptionTextEdit = new QTextEdit(topicDialog);
    descriptionTextEdit->setPlaceholderText(tr("请输入课题描述"));
    descriptionTextEdit->setMaximumHeight(100);
    if (isEdit) {
        descriptionTextEdit->setText(topicInfo.value("description").toString());
    }

    requirementsTextEdit = new QTextEdit(topicDialog);
    requirementsTextEdit->setPlaceholderText(tr("请输入选题要求"));
    requirementsTextEdit->setMaximumHeight(100);
    if (isEdit) {
        requirementsTextEdit->setText(topicInfo.value("requirements").toString());
    }

    maxStudentsLineEdit = new QLineEdit(topicDialog);
    maxStudentsLineEdit->setPlaceholderText(tr("请输入最大小组人数"));
    maxStudentsLineEdit->setValidator(new QIntValidator(1, 20, this));
    if (isEdit) {
        maxStudentsLineEdit->setText(QString::number(topicInfo.value("max_students").toInt()));
    } else {
        maxStudentsLineEdit->setText("5");
    }

    statusComboBox = new QComboBox(topicDialog);
    statusComboBox->addItem(tr("可选题"), 1);
    statusComboBox->addItem(tr("已关闭"), 0);
    if (isEdit) {
        int status = topicInfo.value("status").toInt();
        statusComboBox->setCurrentIndex(status == 1 ? 0 : 1);
    } else {
        statusComboBox->setCurrentIndex(0);
    }

    topicLayout->addRow(tr("课题名称:"), topicNameLineEdit);
    if (userType == 1 || userType == 2) {
        topicLayout->addRow(tr("指导教师:"), topicTeacherComboBox);
    }
    topicLayout->addRow(tr("课题描述:"), descriptionTextEdit);
    topicLayout->addRow(tr("选题要求:"), requirementsTextEdit);
    topicLayout->addRow(tr("最大人数:"), maxStudentsLineEdit);
    topicLayout->addRow(tr("状态:"), statusComboBox);

    dialogLayout->addWidget(topicGroup);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addStretch();

    QPushButton* cancelButton = new QPushButton(tr("取消"), topicDialog);
    connect(cancelButton, &QPushButton::clicked, topicDialog, &QDialog::reject);

    QPushButton* okButton = new QPushButton(isEdit ? tr("保存修改") : tr("添加课题"), topicDialog);
    okButton->setStyleSheet("background-color: #007bff; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    connect(okButton, &QPushButton::clicked, this, isEdit ? &TopicManagementWindow::onEditTopicDialogAccepted : &TopicManagementWindow::onAddTopicDialogAccepted);

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    dialogLayout->addLayout(buttonLayout);

    topicDialog->exec();
}

void TopicManagementWindow::onAddTopicClicked()
{
    showTopicDialog();
}

void TopicManagementWindow::onEditTopicClicked()
{
    QModelIndexList selected = topicTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    QTableWidgetItem* idItem = topicTable->item(row, 0);
    QJsonObject topicInfo = idItem->data(Qt::UserRole).toJsonObject();

    showTopicDialog(topicInfo);
}

void TopicManagementWindow::onDeleteTopicClicked()
{
    QModelIndexList selected = topicTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int topicId = topicTable->item(row, 0)->text().toInt();
    QString topicName = topicTable->item(row, 1)->text();

    int ret = QMessageBox::question(this, tr("确认删除"),
                                   tr("确定要删除课题 '%1' 吗？\n此操作不可恢复，并且会删除相关的选题申请。").arg(topicName),
                                   QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        QJsonObject params;
        params["topic_id"] = topicId;
        if (!currentRequestId.isEmpty()) {
            NetworkManager::getInstance().cancelRequest(currentRequestId);
            currentRequestId.clear();
        }
        currentRequestType = "DELETE_TOPIC";
        currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("DELETE_TOPIC", params);
    }
}

void TopicManagementWindow::onRefreshClicked()
{
    loadTopics();
}

void TopicManagementWindow::onSearchTextChanged(const QString& text)
{
    // 简单的客户端搜索过滤
    for (int i = 0; i < topicTable->rowCount(); ++i) {
        QString topicName = topicTable->item(i, 1)->text();
        bool match = topicName.contains(text, Qt::CaseInsensitive);
        topicTable->setRowHidden(i, !match);
    }
}

void TopicManagementWindow::onStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    loadTopics();
}

void TopicManagementWindow::onTopicResponseReceived(const QString& requestId, const QJsonObject& response)
{
    if (requestId != currentRequestId) {
        return;
    }

    QString status = response.value("status").toString();
    QString message = response.value("message").toString();

    if (status == "success") {
        QJsonObject data = response.value("data").toObject();
        
        if (currentRequestType == "GET_USERS") { // 教师列表
            teachers = data.value("users").toArray();
            // 填充教师下拉框
            teacherComboBox->clear();
            teacherComboBox->addItem(tr("全部教师"), -1);
            for (const QJsonValue& value : teachers) {
                QJsonObject teacher = value.toObject();
                int teacherId = teacher.value("user_id").toInt();
                QString teacherName = teacher.value("real_name").toString();
                teacherComboBox->addItem(teacherName, teacherId);
            }
            currentRequestId.clear();
            currentRequestType.clear();
            loadTopics(); // 加载课题
            return;
        } else if (currentRequestType == "GET_TOPICS") { // 课题列表
            QJsonArray topics = data.value("topics").toArray();
            updateTopicTable(topics);
        } else if (currentRequestType == "GET_USER_INFO") { // 获取用户信息（教师）
            QJsonObject userInfo = data.value("user_info").toObject();
            int teacherId = userInfo.value("teacher_id").toInt();
            
            QJsonObject params;
            params["status"] = statusFilterComboBox->currentData().toInt();
            params["teacher_id"] = teacherId;
            
            currentRequestId.clear();
            currentRequestType.clear();
            currentRequestType = "GET_TOPICS";
            currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_TOPICS", params);
            return;
        } else if (currentRequestType == "ADD_TOPIC") { // 添加课题
            QMessageBox::information(this, tr("成功"), tr("课题添加成功"));
            if (topicDialog) {
                topicDialog->accept();
            }
            currentRequestId.clear();
            currentRequestType.clear();
            loadTopics();
            return;
        } else if (currentRequestType == "UPDATE_TOPIC") { // 更新课题
            QMessageBox::information(this, tr("成功"), tr("课题信息更新成功"));
            if (topicDialog) {
                topicDialog->accept();
            }
            currentRequestId.clear();
            currentRequestType.clear();
            loadTopics();
            return;
        } else if (currentRequestType == "DELETE_TOPIC") { // 删除课题
            QMessageBox::information(this, tr("成功"), tr("课题删除成功"));
            currentRequestId.clear();
            currentRequestType.clear();
            loadTopics();
            return;
        }
    } else {
        QMessageBox::warning(this, tr("失败"), message.isEmpty() ? tr("操作失败，请重试") : message);
    }

    currentRequestId.clear();
    currentRequestType.clear();
}

void TopicManagementWindow::onAddTopicDialogAccepted()
{
    QString topicName = topicNameLineEdit->text().trimmed();
    int teacherId = topicTeacherComboBox->currentData().toInt();
    QString description = descriptionTextEdit->toPlainText().trimmed();
    QString requirements = requirementsTextEdit->toPlainText().trimmed();
    int maxStudents = maxStudentsLineEdit->text().toInt();
    int status = statusComboBox->currentData().toInt();

    if (topicName.isEmpty()) {
        QMessageBox::warning(topicDialog, tr("输入错误"), tr("课题名称不能为空"));
        return;
    }

    if (userType == 1 && teacherId <= 0) {
        QMessageBox::warning(topicDialog, tr("输入错误"), tr("请选择指导教师"));
        return;
    }

    if (maxStudents <= 0 || maxStudents > 20) {
        QMessageBox::warning(topicDialog, tr("输入错误"), tr("最大人数必须在1-20之间"));
        return;
    }

    QJsonObject topicInfo;
    topicInfo["topic_name"] = topicName;
    topicInfo["teacher_id"] = teacherId;
    topicInfo["description"] = description;
    topicInfo["requirements"] = requirements;
    topicInfo["max_students"] = maxStudents;
    topicInfo["status"] = status;

    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    currentRequestType = "ADD_TOPIC";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("ADD_TOPIC", topicInfo);
}

void TopicManagementWindow::onEditTopicDialogAccepted()
{
    QString topicName = topicNameLineEdit->text().trimmed();
    int teacherId = topicTeacherComboBox->currentData().toInt();
    QString description = descriptionTextEdit->toPlainText().trimmed();
    QString requirements = requirementsTextEdit->toPlainText().trimmed();
    int maxStudents = maxStudentsLineEdit->text().toInt();
    int status = statusComboBox->currentData().toInt();

    if (topicName.isEmpty()) {
        QMessageBox::warning(topicDialog, tr("输入错误"), tr("课题名称不能为空"));
        return;
    }

    if (userType == 1 && teacherId <= 0) {
        QMessageBox::warning(topicDialog, tr("输入错误"), tr("请选择指导教师"));
        return;
    }

    if (maxStudents <= 0 || maxStudents > 20) {
        QMessageBox::warning(topicDialog, tr("输入错误"), tr("最大人数必须在1-20之间"));
        return;
    }

    // 获取当前编辑的课题ID
    QModelIndexList selected = topicTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(topicDialog, tr("错误"), tr("无法获取课题ID"));
        return;
    }

    int row = selected.first().row();
    int topicId = topicTable->item(row, 0)->text().toInt();

    QJsonObject topicInfo;
    topicInfo["topic_id"] = topicId;
    topicInfo["topic_name"] = topicName;
    topicInfo["teacher_id"] = teacherId;
    topicInfo["description"] = description;
    topicInfo["requirements"] = requirements;
    topicInfo["max_students"] = maxStudents;
    topicInfo["status"] = status;

    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    currentRequestType = "UPDATE_TOPIC";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("UPDATE_TOPIC", topicInfo);
}
