#include "statisticswindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QScrollArea>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QPdfWriter>
#include "networkmanager.h"

StatisticsWindow::StatisticsWindow(int userType, int userId, QWidget *parent) : QWidget(parent),
    userType(userType),
    userId(userId)
{
    initUI();
    initConnections();
    loadStatistics();
}

StatisticsWindow::~StatisticsWindow()
{
}

void StatisticsWindow::initUI()
{
    setWindowTitle(tr("数据统计"));
    setMinimumSize(900, 600);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    QLabel* titleLabel = new QLabel(tr("数据统计"), this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #343a40;");
    mainLayout->addWidget(titleLabel);

    // 顶部工具栏
    QHBoxLayout* toolLayout = new QHBoxLayout();
    toolLayout->setSpacing(10);

    QLabel* statTypeLabel = new QLabel(tr("统计类型:"), this);
    statTypeComboBox = new QComboBox(this);
    statTypeComboBox->addItem(tr("用户统计"), "user");
    statTypeComboBox->addItem(tr("课题统计"), "topic");
    statTypeComboBox->addItem(tr("申请统计"), "application");
    
    // 根据用户类型调整可查看的统计类型
    if (userType == 2) { // 教师
        statTypeComboBox->removeItem(0); // 移除用户统计
    } else if (userType == 3) { // 学生
        statTypeComboBox->removeItem(0); // 移除用户统计
        statTypeComboBox->removeItem(0); // 移除课题统计
        statTypeComboBox->setCurrentIndex(0); // 默认显示申请统计
    }

    exportButton = new QPushButton(tr("导出数据"), this);
    exportButton->setStyleSheet("background-color: #007bff; color: white; padding: 8px 16px; border: none; border-radius: 4px;");
    
    refreshButton = new QPushButton(tr("刷新"), this);
    refreshButton->setStyleSheet("background-color: #6c757d; color: white; padding: 8px 16px; border: none; border-radius: 4px;");

    toolLayout->addWidget(statTypeLabel);
    toolLayout->addWidget(statTypeComboBox);
    toolLayout->addStretch();
    toolLayout->addWidget(exportButton);
    toolLayout->addWidget(refreshButton);

    mainLayout->addLayout(toolLayout);

    // 图表容器
    chartContainer = new QWidget(this);
    chartContainer->setStyleSheet("border: 1px solid #dee2e6; border-radius: 4px; background-color: white;");
    mainLayout->addWidget(chartContainer);
}

void StatisticsWindow::initConnections()
{
    connect(exportButton, &QPushButton::clicked, this, &StatisticsWindow::onExportClicked);
    connect(refreshButton, &QPushButton::clicked, this, &StatisticsWindow::onRefreshClicked);
    connect(statTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StatisticsWindow::onStatTypeChanged);
    connect(&NetworkManager::getInstance(), &NetworkManager::responseReceived, this, &StatisticsWindow::onResponseReceived);
}

void StatisticsWindow::loadStatistics()
{
    if (!currentRequestId.isEmpty()) {
        NetworkManager::getInstance().cancelRequest(currentRequestId);
        currentRequestId.clear();
    }

    QString statType = statTypeComboBox->currentData().toString();
    currentStatType = statType;

    QJsonObject params;
    params["type"] = statType;
    
    if (userType == 2) { // 教师
        params["teacher_id"] = userId;
    } else if (userType == 3) { // 学生
        params["group_id"] = userId;
    }

    currentRequestType = "GET_STATISTICS";
    currentRequestId = NetworkManager::getInstance().sendRequestWithUserInfo("GET_STATISTICS", params);
}

void StatisticsWindow::showUserStatistics(const QJsonObject& stats)
{
    // 清空容器
    QLayout* oldLayout = chartContainer->layout();
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    QVBoxLayout* layout = new QVBoxLayout(chartContainer);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    // 用户类型分布饼图
    QGroupBox* userTypeGroup = new QGroupBox(tr("用户类型分布"), chartContainer);
    QVBoxLayout* userTypeLayout = new QVBoxLayout(userTypeGroup);

    QPieSeries* userTypeSeries = new QPieSeries();  // 不设父对象
    userTypeSeries->append(tr("管理员: %1").arg(stats.value("admin_count").toInt()),
                           stats.value("admin_count").toInt());
    userTypeSeries->append(tr("教师: %1").arg(stats.value("teacher_count").toInt()),
                           stats.value("teacher_count").toInt());
    userTypeSeries->append(tr("学生小组: %1").arg(stats.value("student_count").toInt()),
                           stats.value("student_count").toInt());

    for (QPieSlice* slice : userTypeSeries->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
    }

    QChart* userTypeChart = new QChart();  // 不设父对象
    userTypeChart->addSeries(userTypeSeries);
    userTypeChart->setTitle(tr("用户类型分布"));
    userTypeChart->legend()->setAlignment(Qt::AlignBottom);

    QChartView* userTypeChartView = new QChartView(userTypeChart, userTypeGroup);  // 父对象为 groupBox
    userTypeChartView->setRenderHint(QPainter::Antialiasing);
    userTypeChartView->setMinimumHeight(300);
    userTypeLayout->addWidget(userTypeChartView);

    layout->addWidget(userTypeGroup);

    // 用户状态统计表格
    QGroupBox* statusGroup = new QGroupBox(tr("用户状态统计"), chartContainer);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);

    QTableWidget* statusTable = new QTableWidget(statusGroup);
    statusTable->setColumnCount(3);
    statusTable->setHorizontalHeaderLabels({tr("状态"), tr("数量"), tr("占比")});
    statusTable->setRowCount(2);

    int activeCount = stats.value("active_user_count").toInt();
    int inactiveCount = stats.value("inactive_user_count").toInt();
    int totalUsers = activeCount + inactiveCount;

    statusTable->setItem(0, 0, new QTableWidgetItem(tr("正常")));
    statusTable->setItem(0, 1, new QTableWidgetItem(QString::number(activeCount)));
    statusTable->setItem(0, 2, new QTableWidgetItem(QString("%.1f%%").arg(totalUsers > 0 ? (activeCount * 100.0 / totalUsers) : 0)));

    statusTable->setItem(1, 0, new QTableWidgetItem(tr("禁用")));
    statusTable->setItem(1, 1, new QTableWidgetItem(QString::number(inactiveCount)));
    statusTable->setItem(1, 2, new QTableWidgetItem(QString("%.1f%%").arg(totalUsers > 0 ? (inactiveCount * 100.0 / totalUsers) : 0)));

    statusTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    statusLayout->addWidget(statusTable);

    layout->addWidget(statusGroup);
}

void StatisticsWindow::showTopicStatistics(const QJsonObject& stats)
{
    QLayout* oldLayout = chartContainer->layout();
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    QVBoxLayout* layout = new QVBoxLayout(chartContainer);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    QGroupBox* statusGroup = new QGroupBox(tr("课题状态分布"), chartContainer);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);

    QPieSeries* statusSeries = new QPieSeries();
    statusSeries->append(tr("可选题: %1").arg(stats.value("active_topic_count").toInt()),
                         stats.value("active_topic_count").toInt());
    statusSeries->append(tr("已关闭: %1").arg(stats.value("closed_topic_count").toInt()),
                         stats.value("closed_topic_count").toInt());

    for (QPieSlice* slice : statusSeries->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
    }

    QChart* statusChart = new QChart();
    statusChart->addSeries(statusSeries);
    statusChart->setTitle(tr("课题状态分布"));
    statusChart->legend()->setAlignment(Qt::AlignBottom);

    QChartView* statusChartView = new QChartView(statusChart, statusGroup);
    statusChartView->setRenderHint(QPainter::Antialiasing);
    statusChartView->setMinimumHeight(300);
    statusLayout->addWidget(statusChartView);

    layout->addWidget(statusGroup);

    QGroupBox* applyGroup = new QGroupBox(tr("课题申请情况"), chartContainer);
    QVBoxLayout* applyLayout = new QVBoxLayout(applyGroup);

    QTableWidget* applyTable = new QTableWidget(applyGroup);
    applyTable->setColumnCount(4);
    applyTable->setHorizontalHeaderLabels({tr("指标"), tr("数量"), tr("已申请"), tr("已完成")});
    applyTable->setRowCount(1);

    int totalTopics = stats.value("total_topic_count").toInt();
    int appliedTopics = stats.value("applied_topic_count").toInt();
    int completedTopics = stats.value("completed_topic_count").toInt();

    applyTable->setItem(0, 0, new QTableWidgetItem(tr("总数")));
    applyTable->setItem(0, 1, new QTableWidgetItem(QString::number(totalTopics)));
    applyTable->setItem(0, 2, new QTableWidgetItem(QString::number(appliedTopics)));
    applyTable->setItem(0, 3, new QTableWidgetItem(QString::number(completedTopics)));

    applyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    applyLayout->addWidget(applyTable);

    layout->addWidget(applyGroup);
}

void StatisticsWindow::showApplicationStatistics(const QJsonObject& stats)
{
    QLayout* oldLayout = chartContainer->layout();
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    QVBoxLayout* layout = new QVBoxLayout(chartContainer);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    QGroupBox* statusGroup = new QGroupBox(tr("申请状态分布"), chartContainer);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);

    QBarSet* applySet = new QBarSet(tr("申请数量"));
    *applySet << stats.value("pending_count").toInt()
              << stats.value("approved_count").toInt()
              << stats.value("rejected_count").toInt();

    QBarSeries* statusSeries = new QBarSeries();
    statusSeries->append(applySet);

    QChart* statusChart = new QChart();
    statusChart->addSeries(statusSeries);
    statusChart->setTitle(tr("申请状态分布"));

    QStringList categories;
    categories << tr("待审核") << tr("已通过") << tr("已驳回");
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    statusChart->addAxis(axisX, Qt::AlignBottom);
    statusSeries->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText(tr("数量"));
    axisY->setMin(0);
    statusChart->addAxis(axisY, Qt::AlignLeft);
    statusSeries->attachAxis(axisY);

    QChartView* statusChartView = new QChartView(statusChart, statusGroup);
    statusChartView->setRenderHint(QPainter::Antialiasing);
    statusChartView->setMinimumHeight(300);
    statusLayout->addWidget(statusChartView);

    layout->addWidget(statusGroup);

    QGroupBox* detailGroup = new QGroupBox(tr("申请统计详情"), chartContainer);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailGroup);

    QTableWidget* detailTable = new QTableWidget(detailGroup);
    detailTable->setColumnCount(3);
    detailTable->setHorizontalHeaderLabels({tr("指标"), tr("数量"), tr("占比")});
    detailTable->setRowCount(4);

    int totalApps = stats.value("total_count").toInt();
    int pendingCount = stats.value("pending_count").toInt();
    int approvedCount = stats.value("approved_count").toInt();
    int rejectedCount = stats.value("rejected_count").toInt();

    detailTable->setItem(0, 0, new QTableWidgetItem(tr("总申请数")));
    detailTable->setItem(0, 1, new QTableWidgetItem(QString::number(totalApps)));
    detailTable->setItem(0, 2, new QTableWidgetItem(tr("100%")));

    detailTable->setItem(1, 0, new QTableWidgetItem(tr("待审核")));
    detailTable->setItem(1, 1, new QTableWidgetItem(QString::number(pendingCount)));
    detailTable->setItem(1, 2, new QTableWidgetItem(QString("%.1f%%").arg(totalApps > 0 ? (pendingCount * 100.0 / totalApps) : 0)));

    detailTable->setItem(2, 0, new QTableWidgetItem(tr("已通过")));
    detailTable->setItem(2, 1, new QTableWidgetItem(QString::number(approvedCount)));
    detailTable->setItem(2, 2, new QTableWidgetItem(QString("%.1f%%").arg(totalApps > 0 ? (approvedCount * 100.0 / totalApps) : 0)));

    detailTable->setItem(3, 0, new QTableWidgetItem(tr("已驳回")));
    detailTable->setItem(3, 1, new QTableWidgetItem(QString::number(rejectedCount)));
    detailTable->setItem(3, 2, new QTableWidgetItem(QString("%.1f%%").arg(totalApps > 0 ? (rejectedCount * 100.0 / totalApps) : 0)));

    detailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    detailLayout->addWidget(detailTable);

    layout->addWidget(detailGroup);
}

void StatisticsWindow::exportToExcel(const QString& fileName)
{
    // 简单实现导出为CSV格式（Excel可以直接打开）
    exportToCsv(fileName);
}

void StatisticsWindow::exportToCsv(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件进行写入"));
        return;
    }

    QTextStream out(&file);
    
    if (currentStatType == "user") {
        // 用户统计导出
        out << tr("用户统计数据") << "\n\n";
        out << tr("管理员数量") << "," << currentStats.value("admin_count").toInt() << "\n";
        out << tr("教师数量") << "," << currentStats.value("teacher_count").toInt() << "\n";
        out << tr("学生小组数量") << "," << currentStats.value("student_count").toInt() << "\n";
        out << tr("活跃用户数量") << "," << currentStats.value("active_user_count").toInt() << "\n";
        out << tr("禁用用户数量") << "," << currentStats.value("inactive_user_count").toInt() << "\n";
    } else if (currentStatType == "topic") {
        // 课题统计导出
        out << tr("课题统计数据") << "\n\n";
        out << tr("总课题数量") << "," << currentStats.value("total_topic_count").toInt() << "\n";
        out << tr("可选题数量") << "," << currentStats.value("active_topic_count").toInt() << "\n";
        out << tr("已关闭课题数量") << "," << currentStats.value("closed_topic_count").toInt() << "\n";
        out << tr("已申请课题数量") << "," << currentStats.value("applied_topic_count").toInt() << "\n";
        out << tr("已完成课题数量") << "," << currentStats.value("completed_topic_count").toInt() << "\n";
    } else if (currentStatType == "application") {
        // 申请统计导出
        out << tr("申请统计数据") << "\n\n";
        out << tr("总申请数量") << "," << currentStats.value("total_count").toInt() << "\n";
        out << tr("待审核数量") << "," << currentStats.value("pending_count").toInt() << "\n";
        out << tr("已通过数量") << "," << currentStats.value("approved_count").toInt() << "\n";
        out << tr("已驳回数量") << "," << currentStats.value("rejected_count").toInt() << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("成功"), tr("数据已成功导出到:\n%1").arg(fileName));
}

void StatisticsWindow::exportToPdf(const QString& fileName)
{
    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setPageMargins(QMargins(20, 20, 20, 20));
    
    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 设置字体
    QFont titleFont = painter.font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    
    QFont headerFont = painter.font();
    headerFont.setPointSize(12);
    headerFont.setBold(true);
    
    QFont normalFont = painter.font();
    normalFont.setPointSize(10);
    
    int y = 50;
    
    // 标题
    painter.setFont(titleFont);
    painter.drawText(100, y, tr("数据统计报表"));
    y += 30;
    
    // 统计类型
    painter.setFont(headerFont);
    QString statTypeName;
    if (currentStatType == "user") statTypeName = tr("用户统计");
    else if (currentStatType == "topic") statTypeName = tr("课题统计");
    else if (currentStatType == "application") statTypeName = tr("申请统计");
    
    painter.drawText(100, y, tr("统计类型: %1").arg(statTypeName));
    y += 20;
    
    // 日期
    painter.setFont(normalFont);
    QString dateStr = QDateTime::currentDateTime().toString(Qt::ISODate);
    painter.drawText(100, y, tr("生成日期: %1").arg(dateStr));
    y += 30;
    
    // 统计数据
    painter.setFont(headerFont);
    painter.drawText(100, y, tr("统计数据:"));
    y += 20;
    
    painter.setFont(normalFont);
    if (currentStatType == "user") {
        painter.drawText(100, y, tr("管理员数量: %1").arg(currentStats.value("admin_count").toInt())); y += 15;
        painter.drawText(100, y, tr("教师数量: %1").arg(currentStats.value("teacher_count").toInt())); y += 15;
        painter.drawText(100, y, tr("学生小组数量: %1").arg(currentStats.value("student_count").toInt())); y += 15;
        painter.drawText(100, y, tr("活跃用户数量: %1").arg(currentStats.value("active_user_count").toInt())); y += 15;
        painter.drawText(100, y, tr("禁用用户数量: %1").arg(currentStats.value("inactive_user_count").toInt()));
    } else if (currentStatType == "topic") {
        painter.drawText(100, y, tr("总课题数量: %1").arg(currentStats.value("total_topic_count").toInt())); y += 15;
        painter.drawText(100, y, tr("可选题数量: %1").arg(currentStats.value("active_topic_count").toInt())); y += 15;
        painter.drawText(100, y, tr("已关闭课题数量: %1").arg(currentStats.value("closed_topic_count").toInt())); y += 15;
        painter.drawText(100, y, tr("已申请课题数量: %1").arg(currentStats.value("applied_topic_count").toInt())); y += 15;
        painter.drawText(100, y, tr("已完成课题数量: %1").arg(currentStats.value("completed_topic_count").toInt()));
    } else if (currentStatType == "application") {
        painter.drawText(100, y, tr("总申请数量: %1").arg(currentStats.value("total_count").toInt())); y += 15;
        painter.drawText(100, y, tr("待审核数量: %1").arg(currentStats.value("pending_count").toInt())); y += 15;
        painter.drawText(100, y, tr("已通过数量: %1").arg(currentStats.value("approved_count").toInt())); y += 15;
        painter.drawText(100, y, tr("已驳回数量: %1").arg(currentStats.value("rejected_count").toInt()));
    }
    
    painter.end();
    QMessageBox::information(this, tr("成功"), tr("数据已成功导出到:\n%1").arg(fileName));
}

void StatisticsWindow::onExportClicked()
{
    if (currentStats.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("没有可导出的数据，请先刷新数据"));
        return;
    }

    QStringList filters;
    filters << tr("CSV文件 (*.csv)")
            << tr("PDF文件 (*.pdf)")
            << tr("Excel文件 (*.xlsx)");
    
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出数据"), 
                                                   QString(), filters.join(";;"));
    
    if (fileName.isEmpty()) {
        return;
    }

    if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        exportToCsv(fileName);
    } else if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        exportToPdf(fileName);
    } else if (fileName.endsWith(".xlsx", Qt::CaseInsensitive)) {
        exportToExcel(fileName);
    } else {
        // 默认导出为CSV
        exportToCsv(fileName + ".csv");
    }
}

void StatisticsWindow::onRefreshClicked()
{
    loadStatistics();
}

void StatisticsWindow::onStatTypeChanged(int index)
{
    Q_UNUSED(index);
    loadStatistics();
}

void StatisticsWindow::onResponseReceived(const QString& requestId, const QJsonObject& response)
{
    if (requestId != currentRequestId) {
        return;
    }

    QString status = response.value("status").toString();
    QString message = response.value("message").toString();

    if (status == "success") {
        if (currentRequestType == "GET_STATISTICS") {
            QJsonObject data = response.value("data").toObject();
            currentStats = data.value("statistics").toObject();
            
            if (currentStatType == "user") {
                showUserStatistics(currentStats);
            } else if (currentStatType == "topic") {
                showTopicStatistics(currentStats);
            } else if (currentStatType == "application") {
                showApplicationStatistics(currentStats);
            }
        }
    } else {
        QMessageBox::warning(this, tr("失败"), message.isEmpty() ? tr("获取统计数据失败，请重试") : message);
    }

    currentRequestId.clear();
    currentRequestType.clear();
}
