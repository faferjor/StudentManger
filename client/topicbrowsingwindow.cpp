#include "topicbrowsingwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QScrollArea>
#include "networkmanager.h"

TopicBrowsingWindow::TopicBrowsingWindow(int userId, int userType, QWidget *parent) : QWidget(parent),
    userId(userId),
    userType(userType),
    groupId(0)
{
    initUI();
    initConnections();
    loadTeachers();
    loadMyApplications();
}

TopicBrowsingWindow::~TopicBrowsingWindow()
{
    if (applyDialog) {
        delete applyDialog;
    }
}

void TopicBrowsingWindow::initUI()
{
    setWindowTitle(tr("课题浏览"));
    setMinimumSize(1000, 600);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    QLabel* titleLabel = new QLabel(tr("课题浏览与申请"), this);
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
    statusFilterComboBox->setCurrentIndex(1); // 默认只显示可选题

    QLabel* teacherLabel = new QLabel(tr("指导教师:"), this);
    teacherComboBox = new QComboBox(this);
    teacherComboBox->addItem(tr("全部教师"), -1);

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(statusLabel);
    searchLayout->addWidget(statusFilterComboBox);
    searchLayout->addWidget(teacherLabel);
    searchLayout->addWidget(teacherComboBox);
    searchLayout->addStretch();

    mainLayout->addLayout(searchLayout);

    // 课题表格
    topicTable = new QTableWidget(this);
    topicTable->setColumnCount(7);
    QStringList headers = {tr("课题ID"), tr("课题名称"), tr("指导教师"), tr("当前人数"), tr("最大人数"), tr("状态"), tr("操作")};
    topicTable->setHorizontalHeaderLabels(headers);
    topicTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    topicTable->setSelectionMode(QAbstractItemView::SingleSelection);
    topicTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    topicTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    topicTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    topicTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    mainLayout->addWidget(topicTable);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    applyTopicButton = new QPushButton(tr("申请选中课题"), this);
    applyTopicButton->setStyleSheet("background-color: #28a745; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    applyTopicButton->setEnabled(false);
    
    refreshButton = new QPushButton(tr("刷新"), this);
    refreshButton->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 16px; border: none; border-radius: 4px;");

    buttonLayout->addWidget(applyTopicButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void TopicBrowsingWindow::initConnections()
{
    connect(applyTopicButton, &QPushButton::clicked, this, &TopicBrowsingWindow::onApplyTopicClicked);
    connect(refreshButton, &QPushButton::clicked, this, &TopicBrowsingWindow::onRefreshClicked);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &TopicBrowsingWindow::onSearchTextChanged);
    connect(statusFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TopicBrowsingWindow::onRefreshClicked);
    connect(teacherComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TopicBrowsingWindow::onRefreshClicked);
    connect(topicTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !topicTable->selectedItems().isEmpty();
        applyTopicButton->setEnabled(hasSelection);
    });
    connect(&NetworkManager::getInstance(), &NetworkManager::responseReceived, this, &TopicBrowsingWindow::onTopicResponseReceived);
}

void TopicBrowsingWindow::loadTeachers()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    QJsonObject params;
    params["user_type"] = 2;
    currentRequestType = "GET_USERS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_USERS", params);
}

void TopicBrowsingWindow::loadTopics()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    int status = statusFilterComboBox->currentData().toInt();
    int teacherId = teacherComboBox->currentData().toInt();

    QJsonObject params;
    if (status != -1) {
        params["status"] = status;
    }
    if (teacherId != -1) {
        params["teacher_id"] = teacherId;
    }

    currentRequestType = "GET_TOPICS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_TOPICS", params);
}

void TopicBrowsingWindow::loadMyApplications()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    // 需要先获取小组ID
    QJsonObject params;
    params["user_id"] = userId;
    currentRequestType = "GET_USER_INFO";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_USER_INFO", params);
}

void TopicBrowsingWindow::updateTopicTable(const QJsonArray& topics)
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
        // int teacherId = topic.value("teacher_id").toInt();

        QString statusStr = status == 1 ? tr("可选题") : tr("已关闭");
        QString statusColor = status == 1 ? "#28a745" : "#dc3545";

        topicTable->setItem(row, 0, new QTableWidgetItem(QString::number(topicId)));
        topicTable->setItem(row, 1, new QTableWidgetItem(topicName));
        topicTable->setItem(row, 2, new QTableWidgetItem(teacherName));
        topicTable->setItem(row, 3, new QTableWidgetItem(QString::number(currentStudents)));
        topicTable->setItem(row, 4, new QTableWidgetItem(QString::number(maxStudents)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusStr);
        topicTable->setItem(row, 5, statusItem);

        // 操作按钮
        QWidget* buttonWidget = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

        QPushButton* detailButton = new QPushButton(tr("详情"));
        detailButton->setStyleSheet("background-color: #007bff; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
        detailButton->setFixedSize(60, 25);
        connect(detailButton, &QPushButton::clicked, this, [=]() {
            showTopicDetail(topic);
        });

        QPushButton* applyButton = new QPushButton(tr("申请"));
        applyButton->setStyleSheet("background-color: #28a745; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
        applyButton->setFixedSize(60, 25);
        
        bool canApply = (status == 1 && currentStudents < maxStudents && !appliedTopicIds.contains(topicId));
        applyButton->setEnabled(canApply);
        if (!canApply) {
            if (appliedTopicIds.contains(topicId)) {
                applyButton->setText(tr("已申请"));
                applyButton->setStyleSheet("background-color: #6c757d; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
            } else if (currentStudents >= maxStudents) {
                applyButton->setText(tr("已满"));
                applyButton->setStyleSheet("background-color: #dc3545; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
            } else if (status != 1) {
                applyButton->setText(tr("已关闭"));
                applyButton->setStyleSheet("background-color: #6c757d; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
            }
        } else {
            connect(applyButton, &QPushButton::clicked, this, [=]() {
                QJsonObject topicInfo = topic;
                onApplyTopicClicked();
                topicTable->selectRow(row);
            });
        }

        buttonLayout->addWidget(detailButton);
        buttonLayout->addWidget(applyButton);
        buttonWidget->setLayout(buttonLayout);
        topicTable->setCellWidget(row, 6, buttonWidget);

        // 存储课题数据到行的隐藏数据
        QTableWidgetItem* idItem = topicTable->item(row, 0);
        idItem->setData(Qt::UserRole, QVariant::fromValue(topic));
    }
}

void TopicBrowsingWindow::updateAppliedTopics()
{
    appliedTopicIds.clear();
    for (const QJsonValue& value : myApplications) {
        QJsonObject app = value.toObject();
        int topicId = app.value("topic_id").toInt();
        int status = app.value("status").toInt();
        if (status == 0 || status == 1) { // 待审核或已通过的申请
            appliedTopicIds.insert(topicId);
        }
    }
}

void TopicBrowsingWindow::showTopicDetail(const QJsonObject& topicInfo)
{
    QDialog* detailDialog = new QDialog(this);
    detailDialog->setWindowTitle(tr("课题详情"));
    detailDialog->setMinimumWidth(500);
    detailDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(detailDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    // 课题基本信息
    QGroupBox* basicGroup = new QGroupBox(tr("基本信息"), detailDialog);
    QFormLayout* basicLayout = new QFormLayout(basicGroup);
    basicLayout->setSpacing(10);

    QLabel* topicNameLabel = new QLabel(topicInfo.value("topic_name").toString(), detailDialog);
    topicNameLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    basicLayout->addRow(tr("课题名称:"), topicNameLabel);

    QLabel* teacherLabel = new QLabel(topicInfo.value("teacher_name").toString(), detailDialog);
    basicLayout->addRow(tr("指导教师:"), teacherLabel);

    QString studentCount = QString("%1/%2").arg(topicInfo.value("current_students").toInt())
                                        .arg(topicInfo.value("max_students").toInt());
    QLabel* countLabel = new QLabel(studentCount, detailDialog);
    basicLayout->addRow(tr("人数限制:"), countLabel);

    QString statusStr = topicInfo.value("status").toInt() == 1 ? tr("可选题") : tr("已关闭");
    QLabel* statusLabel = new QLabel(statusStr, detailDialog);
    statusLabel->setStyleSheet(topicInfo.value("status").toInt() == 1 ? "color: #28a745;" : "color: #dc3545;");
    basicLayout->addRow(tr("状态:"), statusLabel);

    dialogLayout->addWidget(basicGroup);

    // 课题描述
    QGroupBox* descGroup = new QGroupBox(tr("课题描述"), detailDialog);
    QVBoxLayout* descLayout = new QVBoxLayout(descGroup);
    descLayout->setSpacing(10);

    QTextEdit* descTextEdit = new QTextEdit(detailDialog);
    descTextEdit->setReadOnly(true);
    descTextEdit->setText(topicInfo.value("description").toString().isEmpty() ? tr("暂无描述") : topicInfo.value("description").toString());
    descTextEdit->setMaximumHeight(120);
    descLayout->addWidget(descTextEdit);

    dialogLayout->addWidget(descGroup);

    // 选题要求
    QGroupBox* reqGroup = new QGroupBox(tr("选题要求"), detailDialog);
    QVBoxLayout* reqLayout = new QVBoxLayout(reqGroup);
    reqLayout->setSpacing(10);

    QTextEdit* reqTextEdit = new QTextEdit(detailDialog);
    reqTextEdit->setReadOnly(true);
    reqTextEdit->setText(topicInfo.value("requirements").toString().isEmpty() ? tr("暂无要求") : topicInfo.value("requirements").toString());
    reqTextEdit->setMaximumHeight(120);
    reqLayout->addWidget(reqTextEdit);

    dialogLayout->addWidget(reqGroup);

    // 创建时间
    QLabel* createTimeLabel = new QLabel(tr("创建时间: %1").arg(topicInfo.value("create_time").toString().left(19)), detailDialog);
    createTimeLabel->setStyleSheet("font-size: 12px; color: #6c757d;");
    dialogLayout->addWidget(createTimeLabel);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addStretch();

    QPushButton* closeButton = new QPushButton(tr("关闭"), detailDialog);
    connect(closeButton, &QPushButton::clicked, detailDialog, &QDialog::close);

    buttonLayout->addWidget(closeButton);
    dialogLayout->addLayout(buttonLayout);

    detailDialog->exec();
}

void TopicBrowsingWindow::onApplyTopicClicked()
{
    QModelIndexList selected = topicTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要申请的课题"));
        return;
    }

    int row = selected.first().row();
    QTableWidgetItem* idItem = topicTable->item(row, 0);
    QJsonObject topicInfo = idItem->data(Qt::UserRole).toJsonObject();

    int topicId = topicInfo.value("topic_id").toInt();
    int status = topicInfo.value("status").toInt();
    int currentStudents = topicInfo.value("current_students").toInt();
    int maxStudents = topicInfo.value("max_students").toInt();

    if (status != 1) {
        QMessageBox::warning(this, tr("提示"), tr("该课题目前不可申请"));
        return;
    }

    if (currentStudents >= maxStudents) {
        QMessageBox::warning(this, tr("提示"), tr("该课题人数已满，无法申请"));
        return;
    }

    if (appliedTopicIds.contains(topicId)) {
        QMessageBox::warning(this, tr("提示"), tr("您已经申请过该课题，请勿重复申请"));
        return;
    }

    // 显示申请对话框
    applyDialog = new QDialog(this);
    applyDialog->setWindowTitle(tr("申请课题"));
    applyDialog->setMinimumWidth(400);
    applyDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(applyDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel(tr("申请课题: %1").arg(topicInfo.value("topic_name").toString()), applyDialog);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    dialogLayout->addWidget(titleLabel);

    QGroupBox* applyGroup = new QGroupBox(tr("申请信息"), applyDialog);
    QFormLayout* applyLayout = new QFormLayout(applyGroup);
    applyLayout->setSpacing(10);

    reasonTextEdit = new QTextEdit(applyDialog);
    reasonTextEdit->setPlaceholderText(tr("请输入申请理由（选填）"));
    reasonTextEdit->setMaximumHeight(100);
    applyLayout->addRow(tr("申请理由:"), reasonTextEdit);

    dialogLayout->addWidget(applyGroup);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addStretch();

    QPushButton* cancelButton = new QPushButton(tr("取消"), applyDialog);
    connect(cancelButton, &QPushButton::clicked, applyDialog, &QDialog::reject);

    QPushButton* okButton = new QPushButton(tr("确认申请"), applyDialog);
    okButton->setStyleSheet("background-color: #28a745; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    connect(okButton, &QPushButton::clicked, this, &TopicBrowsingWindow::onApplyDialogAccepted);

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    dialogLayout->addLayout(buttonLayout);

    applyDialog->exec();
}

void TopicBrowsingWindow::onRefreshClicked()
{
    loadTopics();
}

void TopicBrowsingWindow::onSearchTextChanged(const QString& text)
{
    // 简单的客户端搜索过滤
    for (int i = 0; i < topicTable->rowCount(); ++i) {
        QString topicName = topicTable->item(i, 1)->text();
        bool match = topicName.contains(text, Qt::CaseInsensitive);
        topicTable->setRowHidden(i, !match);
    }
}

void TopicBrowsingWindow::onStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    loadTopics();
}

void TopicBrowsingWindow::onTeacherFilterChanged(int index)
{
    Q_UNUSED(index);
    loadTopics();
}

void TopicBrowsingWindow::onTopicResponseReceived(const QString& requestId, const QJsonObject& response)
{
    if (requestId != currentRequestId) return;

    QString status = response.value("status").toString();
    QString message = response.value("message").toString();

    if (status == "success") {
        QJsonObject data = response.value("data").toObject();

        if (currentRequestType == "GET_USERS") {
            QJsonArray teachers = data.value("users").toArray();
            teacherComboBox->clear();
            teacherComboBox->addItem(tr("全部教师"), -1);
            for (const QJsonValue& value : teachers) {
                QJsonObject teacher = value.toObject();
                int tid = teacher.value("user_id").toInt();
                QString tname = teacher.value("real_name").toString();
                teacherComboBox->addItem(tname, tid);
            }
            currentRequestId.clear();
            currentRequestType.clear();
            loadTopics();
            return;
        } else if (currentRequestType == "GET_TOPICS") {
            QJsonArray topics = data.value("topics").toArray();
            updateTopicTable(topics);
        } else if (currentRequestType == "GET_USER_INFO") {
            QJsonObject userInfo = data.value("user_info").toObject();
            groupId = userInfo.value("group_id").toInt();
            QJsonObject params;
            params["group_id"] = groupId;
            currentRequestId.clear();
            currentRequestType.clear();
            currentRequestType = "GET_APPLICATIONS";
            currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_APPLICATIONS", params);
            return;
        } else if (currentRequestType == "GET_APPLICATIONS") {
            myApplications = data.value("applications").toArray();
            updateAppliedTopics();
            currentRequestId.clear();
            currentRequestType.clear();
            loadTopics();
            return;
        } else if (currentRequestType == "ADD_APPLICATION") {
            QMessageBox::information(this, tr("成功"), tr("课题申请提交成功，等待教师审核"));
            if (applyDialog) applyDialog->accept();
            currentRequestId.clear();
            currentRequestType.clear();
            loadMyApplications();
            return;
        }
    } else {
        QMessageBox::warning(this, tr("失败"), message.isEmpty() ? tr("操作失败，请重试") : message);
    }

    currentRequestId.clear();
    currentRequestType.clear();
}
void TopicBrowsingWindow::onApplyDialogAccepted()
{
    QModelIndexList selected = topicTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    int row = selected.first().row();
    QTableWidgetItem* idItem = topicTable->item(row, 0);
    QJsonObject topicInfo = idItem->data(Qt::UserRole).toJsonObject();

    int topicId = topicInfo.value("topic_id").toInt();
    QString reason = reasonTextEdit->toPlainText().trimmed();

    QJsonObject applicationInfo;
    applicationInfo["group_id"] = groupId;
    applicationInfo["topic_id"] = topicId;
    if (!reason.isEmpty()) applicationInfo["reason"] = reason;

    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }
    currentRequestType = "ADD_APPLICATION";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("ADD_APPLICATION", applicationInfo);
}
