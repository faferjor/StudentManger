#include "applicationreviewwindow.h"
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

ApplicationReviewWindow::ApplicationReviewWindow(int userType, int userId, QWidget *parent) : QWidget(parent),
    userType(userType),
    userId(userId),
    teacherId(0)
{
    initUI();
    initConnections();
    
    if (userType == 2) { // 教师
        // 需要先获取教师ID
        QJsonObject params;
        params["user_id"] = userId;
        currentRequestType = "GET_USER_INFO";
        currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_USER_INFO", params);
    } else { // 管理员
        loadTopics();
        loadApplications();
    }
}

ApplicationReviewWindow::~ApplicationReviewWindow()
{
}

void ApplicationReviewWindow::initUI()
{
    setWindowTitle(userType == 1 ? tr("申请管理") : tr("申请审核"));
    setMinimumSize(1100, 600);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    QString title = userType == 1 ? tr("申请管理") : tr("申请审核");
    QLabel* titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #343a40;");
    mainLayout->addWidget(titleLabel);

    // 搜索和筛选区域
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(10);

    QLabel* searchLabel = new QLabel(tr("搜索:"), this);
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText(tr("输入小组名称或课题名称"));
    searchLineEdit->setMinimumWidth(200);

    QLabel* statusLabel = new QLabel(tr("状态:"), this);
    statusFilterComboBox = new QComboBox(this);
    statusFilterComboBox->addItem(tr("全部"), -1);
    statusFilterComboBox->addItem(tr("待审核"), 0);
    statusFilterComboBox->addItem(tr("已通过"), 1);
    statusFilterComboBox->addItem(tr("已驳回"), 2);
    statusFilterComboBox->setCurrentIndex(1); // 默认显示待审核

    if (userType == 1) { // 管理员
        QLabel* topicLabel = new QLabel(tr("课题:"), this);
        topicFilterComboBox = new QComboBox(this);
        topicFilterComboBox->addItem(tr("全部课题"), -1);

        searchLayout->addWidget(searchLabel);
        searchLayout->addWidget(searchLineEdit);
        searchLayout->addWidget(statusLabel);
        searchLayout->addWidget(statusFilterComboBox);
        searchLayout->addWidget(topicLabel);
        searchLayout->addWidget(topicFilterComboBox);
    } else {
        searchLayout->addWidget(searchLabel);
        searchLayout->addWidget(searchLineEdit);
        searchLayout->addWidget(statusLabel);
        searchLayout->addWidget(statusFilterComboBox);
    }

    searchLayout->addStretch();
    mainLayout->addLayout(searchLayout);

    // 申请表格
    applicationTable = new QTableWidget(this);
    int columnCount = userType == 1 ? 9 : 8;
    applicationTable->setColumnCount(columnCount);
    
    QStringList headers;
    if (userType == 1) {
        headers = {tr("申请ID"), tr("小组名称"), tr("课题名称"), tr("指导教师"), tr("申请时间"), tr("状态"), tr("审核时间"), tr("审核人"), tr("操作")};
    } else {
        headers = {tr("申请ID"), tr("小组名称"), tr("课题名称"), tr("申请时间"), tr("状态"), tr("审核时间"), tr("审核结果"), tr("操作")};
    }
    
    applicationTable->setHorizontalHeaderLabels(headers);
    applicationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    applicationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    applicationTable->setSelectionMode(QAbstractItemView::SingleSelection);
    applicationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    applicationTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    applicationTable->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    mainLayout->addWidget(applicationTable);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    approveButton = new QPushButton(tr("通过选中申请"), this);
    approveButton->setStyleSheet("background-color: #28a745; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    approveButton->setEnabled(false);
    
    rejectButton = new QPushButton(tr("驳回选中申请"), this);
    rejectButton->setStyleSheet("background-color: #dc3545; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    rejectButton->setEnabled(false);
    
    refreshButton = new QPushButton(tr("刷新"), this);
    refreshButton->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 16px; border: none; border-radius: 4px;");

    buttonLayout->addWidget(approveButton);
    buttonLayout->addWidget(rejectButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void ApplicationReviewWindow::initConnections()
{
    connect(approveButton, &QPushButton::clicked, this, &ApplicationReviewWindow::onApproveClicked);
    connect(rejectButton, &QPushButton::clicked, this, &ApplicationReviewWindow::onRejectClicked);
    connect(refreshButton, &QPushButton::clicked, this, &ApplicationReviewWindow::onRefreshClicked);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &ApplicationReviewWindow::onSearchTextChanged);
    connect(statusFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ApplicationReviewWindow::onRefreshClicked);
    if (topicFilterComboBox) {
        connect(topicFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ApplicationReviewWindow::onRefreshClicked);
    }
    connect(applicationTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !applicationTable->selectedItems().isEmpty();
        bool canReview = false;
        
        if (hasSelection) {
            QModelIndexList selected = applicationTable->selectionModel()->selectedRows();
            int row = selected.first().row();
            QString status = applicationTable->item(row, 5)->text();
            canReview = (status == tr("待审核"));
        }
        
        approveButton->setEnabled(hasSelection && canReview);
        rejectButton->setEnabled(hasSelection && canReview);
    });
    connect(&NetworkManager::getInstance(), &NetworkManager::responseReceived, this, &ApplicationReviewWindow::onResponseReceived);
}

void ApplicationReviewWindow::loadTopics()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    QJsonObject params;
    params["status"] = 1;
    currentRequestType = "GET_TOPICS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_TOPICS", params);
}

void ApplicationReviewWindow::loadApplications()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    int status = statusFilterComboBox->currentData().toInt();
    int topicId = -1;
    int groupId = -1;
    
    if (userType == 1) { // 管理员
        if (topicFilterComboBox) {
            topicId = topicFilterComboBox->currentData().toInt();
        }
    } else if (userType == 2) { // 教师
        topicId = teacherId;
    }

    QJsonObject params;
    if (status != -1) {
        params["status"] = status;
    }
    if (topicId != -1) {
        params["topic_id"] = topicId;
    }
    if (groupId != -1) {
        params["group_id"] = groupId;
    }

    currentRequestType = "GET_APPLICATIONS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_APPLICATIONS", params);
}

void ApplicationReviewWindow::updateApplicationTable(const QJsonArray& applications)
{
    applicationTable->setRowCount(0);

    for (const QJsonValue& value : applications) {
        QJsonObject app = value.toObject();
        int row = applicationTable->rowCount();
        applicationTable->insertRow(row);

        int appId = app.value("application_id").toInt();
        QString groupName = app.value("group_name").toString();
        QString topicName = app.value("topic_name").toString();
        QString teacherName = app.value("teacher_name").toString();
        QString applyTime = app.value("application_time").toString().left(19);
        int status = app.value("status").toInt();
        QString reviewTime = app.value("review_time").toString();
        QString reviewerName = app.value("reviewer_name").toString();
        QString rejectReason = app.value("reject_reason").toString();

        QString statusStr;
        QString statusColor;
        QString resultStr;
        
        switch (status) {
        case 0:
            statusStr = tr("待审核");
            statusColor = "#ffc107";
            resultStr = tr("等待审核");
            break;
        case 1:
            statusStr = tr("已通过");
            statusColor = "#28a745";
            resultStr = tr("审核通过");
            break;
        case 2:
            statusStr = tr("已驳回");
            statusColor = "#dc3545";
            resultStr = tr("审核驳回") + (rejectReason.isEmpty() ? "" : QString("：%1").arg(rejectReason));
            break;
        default:
            statusStr = tr("未知");
            statusColor = "#6c757d";
            resultStr = tr("未知状态");
            break;
        }

        int column = 0;
        applicationTable->setItem(row, column++, new QTableWidgetItem(QString::number(appId)));
        applicationTable->setItem(row, column++, new QTableWidgetItem(groupName));
        applicationTable->setItem(row, column++, new QTableWidgetItem(topicName));
        
        if (userType == 1) { // 管理员显示指导教师
            applicationTable->setItem(row, column++, new QTableWidgetItem(teacherName));
        }
        
        applicationTable->setItem(row, column++, new QTableWidgetItem(applyTime));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusStr);
        applicationTable->setItem(row, column++, statusItem);
        
        QString reviewTimeStr = reviewTime.isEmpty() ? tr("未审核") : reviewTime.left(19);
        applicationTable->setItem(row, column++, new QTableWidgetItem(reviewTimeStr));
        
        if (userType == 1) { // 管理员显示审核人
            QString reviewerStr = reviewerName.isEmpty() ? tr("未审核") : reviewerName;
            applicationTable->setItem(row, column++, new QTableWidgetItem(reviewerStr));
        } else { // 教师显示审核结果
            applicationTable->setItem(row, column++, new QTableWidgetItem(resultStr));
        }

        // 操作按钮
        QWidget* buttonWidget = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

        QPushButton* detailButton = new QPushButton(tr("详情"));
        detailButton->setStyleSheet("background-color: #007bff; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
        detailButton->setFixedSize(60, 25);
        connect(detailButton, &QPushButton::clicked, this, [=]() {
            showApplicationDetail(app);
        });

        if (status == 0) { // 待审核
            QPushButton* approveBtn = new QPushButton(tr("通过"));
            approveBtn->setStyleSheet("background-color: #28a745; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
            approveBtn->setFixedSize(60, 25);
            connect(approveBtn, &QPushButton::clicked, this, [=]() {
                updateApplicationStatus(appId, 1);
            });

            QPushButton* rejectBtn = new QPushButton(tr("驳回"));
            rejectBtn->setStyleSheet("background-color: #dc3545; color: white; padding: 4px 8px; border: none; border-radius: 3px; font-size: 12px;");
            rejectBtn->setFixedSize(60, 25);
            connect(rejectBtn, &QPushButton::clicked, this, [=]() {
                showRejectDialog(app);
            });

            buttonLayout->addWidget(detailButton);
            buttonLayout->addWidget(approveBtn);
            buttonLayout->addWidget(rejectBtn);
        } else {
            buttonLayout->addWidget(detailButton);
        }

        buttonWidget->setLayout(buttonLayout);
        applicationTable->setCellWidget(row, column++, buttonWidget);

        // 存储申请数据到行的隐藏数据
        QTableWidgetItem* idItem = applicationTable->item(row, 0);
        idItem->setData(Qt::UserRole, QVariant::fromValue(app));
    }
}

void ApplicationReviewWindow::showApplicationDetail(const QJsonObject& appInfo)
{
    QDialog* detailDialog = new QDialog(this);
    detailDialog->setWindowTitle(tr("申请详情"));
    detailDialog->setMinimumWidth(500);
    detailDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(detailDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    // 申请基本信息
    QGroupBox* basicGroup = new QGroupBox(tr("申请信息"), detailDialog);
    QFormLayout* basicLayout = new QFormLayout(basicGroup);
    basicLayout->setSpacing(10);

    basicLayout->addRow(tr("申请ID:"), new QLabel(QString::number(appInfo.value("application_id").toInt()), detailDialog));
    basicLayout->addRow(tr("小组名称:"), new QLabel(appInfo.value("group_name").toString(), detailDialog));
    basicLayout->addRow(tr("课题名称:"), new QLabel(appInfo.value("topic_name").toString(), detailDialog));
    basicLayout->addRow(tr("指导教师:"), new QLabel(appInfo.value("teacher_name").toString(), detailDialog));
    
    QString applyTime = appInfo.value("application_time").toString().left(19);
    basicLayout->addRow(tr("申请时间:"), new QLabel(applyTime, detailDialog));

    int status = appInfo.value("status").toInt();
    QString statusStr;
    switch (status) {
    case 0: statusStr = tr("待审核"); break;
    case 1: statusStr = tr("已通过"); break;
    case 2: statusStr = tr("已驳回"); break;
    default: statusStr = tr("未知"); break;
    }
    basicLayout->addRow(tr("当前状态:"), new QLabel(statusStr, detailDialog));

    dialogLayout->addWidget(basicGroup);

    // 审核信息
    if (status != 0) {
        QGroupBox* reviewGroup = new QGroupBox(tr("审核信息"), detailDialog);
        QFormLayout* reviewLayout = new QFormLayout(reviewGroup);
        reviewLayout->setSpacing(10);

        QString reviewTime = appInfo.value("review_time").toString().left(19);
        reviewLayout->addRow(tr("审核时间:"), new QLabel(reviewTime, detailDialog));
        reviewLayout->addRow(tr("审核人:"), new QLabel(appInfo.value("reviewer_name").toString(), detailDialog));

        if (status == 2) { // 已驳回
            QString rejectReason = appInfo.value("reject_reason").toString();
            if (rejectReason.isEmpty()) {
                rejectReason = tr("未填写驳回理由");
            }
            QLabel* reasonLabel = new QLabel(rejectReason, detailDialog);
            reasonLabel->setWordWrap(true);
            reviewLayout->addRow(tr("驳回理由:"), reasonLabel);
        }

        dialogLayout->addWidget(reviewGroup);
    }

    // 申请理由
    QString reason = appInfo.value("reason").toString();
    if (!reason.isEmpty()) {
        QGroupBox* reasonGroup = new QGroupBox(tr("申请理由"), detailDialog);
        QVBoxLayout* reasonLayout = new QVBoxLayout(reasonGroup);
        reasonLayout->setSpacing(10);

        QLabel* reasonLabel = new QLabel(reason, detailDialog);
        reasonLabel->setWordWrap(true);
        reasonLayout->addWidget(reasonLabel);

        dialogLayout->addWidget(reasonGroup);
    }

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

void ApplicationReviewWindow::showRejectDialog(const QJsonObject& appInfo)
{
    QDialog* rejectDialog = new QDialog(this);
    rejectDialog->setWindowTitle(tr("驳回申请"));
    rejectDialog->setMinimumWidth(400);
    rejectDialog->setModal(true);

    QVBoxLayout* dialogLayout = new QVBoxLayout(rejectDialog);
    dialogLayout->setContentsMargins(20, 20, 20, 20);
    dialogLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel(tr("驳回申请：%1").arg(appInfo.value("topic_name").toString()), rejectDialog);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    dialogLayout->addWidget(titleLabel);

    QLabel* hintLabel = new QLabel(tr("请输入驳回理由（选填）："), rejectDialog);
    dialogLayout->addWidget(hintLabel);

    QTextEdit* reasonTextEdit = new QTextEdit(rejectDialog);
    reasonTextEdit->setPlaceholderText(tr("请输入驳回理由，说明驳回原因"));
    reasonTextEdit->setMaximumHeight(100);
    dialogLayout->addWidget(reasonTextEdit);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addStretch();

    QPushButton* cancelButton = new QPushButton(tr("取消"), rejectDialog);
    connect(cancelButton, &QPushButton::clicked, rejectDialog, &QDialog::reject);

    QPushButton* confirmButton = new QPushButton(tr("确认驳回"), rejectDialog);
    confirmButton->setStyleSheet("background-color: #dc3545; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    connect(confirmButton, &QPushButton::clicked, this, [=]() {
        int appId = appInfo.value("application_id").toInt();
        QString reason = reasonTextEdit->toPlainText().trimmed();
        updateApplicationStatus(appId, 2, reason);
        rejectDialog->accept();
    });

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);

    dialogLayout->addLayout(buttonLayout);

    rejectDialog->exec();
}

void ApplicationReviewWindow::updateApplicationStatus(int applicationId, int status, const QString& rejectReason)
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    QJsonObject params;
    params["application_id"] = applicationId;
    params["status"] = status;
    params["reviewer_id"] = userId;
    if (!rejectReason.isEmpty()) {
        params["reject_reason"] = rejectReason;
    }

    currentRequestType = "UPDATE_APPLICATION_STATUS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("UPDATE_APPLICATION_STATUS", params);
}

void ApplicationReviewWindow::onApproveClicked()
{
    QModelIndexList selected = applicationTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要审核的申请"));
        return;
    }

    int row = selected.first().row();
    int appId = applicationTable->item(row, 0)->text().toInt();
    QString status = applicationTable->item(row, 5)->text();

    if (status != tr("待审核")) {
        QMessageBox::warning(this, tr("提示"), tr("只有待审核的申请才能进行审核操作"));
        return;
    }

    int ret = QMessageBox::question(this, tr("确认通过"),
                                   tr("确定要通过该申请吗？"),
                                   QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        updateApplicationStatus(appId, 1);
    }
}

void ApplicationReviewWindow::onRejectClicked()
{
    QModelIndexList selected = applicationTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要审核的申请"));
        return;
    }

    int row = selected.first().row();
    QTableWidgetItem* idItem = applicationTable->item(row, 0);
    QJsonObject appInfo = idItem->data(Qt::UserRole).toJsonObject();

    showRejectDialog(appInfo);
}

void ApplicationReviewWindow::onRefreshClicked()
{
    loadApplications();
}

void ApplicationReviewWindow::onStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    loadApplications();
}

void ApplicationReviewWindow::onTopicFilterChanged(int index)
{
    Q_UNUSED(index);
    loadApplications();
}

void ApplicationReviewWindow::onSearchTextChanged(const QString& text)
{
    // 简单的客户端搜索过滤
    for (int i = 0; i < applicationTable->rowCount(); ++i) {
        QString groupName = applicationTable->item(i, 1)->text();
        QString topicName = applicationTable->item(i, 2)->text();
        bool match = groupName.contains(text, Qt::CaseInsensitive) || topicName.contains(text, Qt::CaseInsensitive);
        applicationTable->setRowHidden(i, !match);
    }
}

void ApplicationReviewWindow::onResponseReceived(const QString& requestId, const QJsonObject& response)
{
    if (requestId != currentRequestId) {
        return;
    }

    QString status = response.value("status").toString();
    QString message = response.value("message").toString();

    if (status == "success") {
        QJsonObject data = response.value("data").toObject();
        
        if (currentRequestType == "GET_USER_INFO") { // 用户信息
            QJsonObject userInfo = data.value("user_info").toObject();
            teacherId = userInfo.value("teacher_id").toInt();
            currentRequestId.clear();
            currentRequestType.clear();
            loadApplications(); // 加载申请
            return;
        } else if (currentRequestType == "GET_TOPICS") { // 课题列表
            QJsonArray topics = data.value("topics").toArray();

            topicFilterComboBox->clear();
            topicFilterComboBox->addItem(tr("全部课题"), -1);

            for (const QJsonValue& value : topics) {
                QJsonObject topic = value.toObject();
                int topicId = topic.value("topic_id").toInt();
                QString topicName = topic.value("topic_name").toString();
                topicFilterComboBox->addItem(topicName, topicId);
            }
            currentRequestId.clear();
            currentRequestType.clear();
            loadApplications(); // 加载申请
            return;
        } else if (currentRequestType == "GET_APPLICATIONS") { // 申请列表
            QJsonArray applications = data.value("applications").toArray();
            updateApplicationTable(applications);
        } else if (currentRequestType == "UPDATE_APPLICATION_STATUS") { // 更新审核状态
            QMessageBox::information(this, tr("成功"), tr("审核操作完成"));
            currentRequestId.clear();
            currentRequestType.clear();
            loadApplications(); // 重新加载申请
            return;
        }
    } else {
        QMessageBox::warning(this, tr("失败"), message.isEmpty() ? tr("操作失败，请重试") : message);
    }

    currentRequestId.clear();
    currentRequestType.clear();
}
