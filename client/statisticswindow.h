#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QLineSeries>
#include <QCategoryAxis>
#include <QBarCategoryAxis>
#include <QValueAxis>

class StatisticsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticsWindow(int userType, int userId, QWidget *parent = nullptr);
    ~StatisticsWindow();

private slots:
    void onExportClicked();
    void onRefreshClicked();
    void onStatTypeChanged(int index);
    void onResponseReceived(const QString& requestId, const QJsonObject& response);

private:
    void initUI();
    void initConnections();
    void loadStatistics();
    void showUserStatistics(const QJsonObject& stats);
    void showTopicStatistics(const QJsonObject& stats);
    void showApplicationStatistics(const QJsonObject& stats);
    void exportToExcel(const QString& fileName);
    void exportToCsv(const QString& fileName);
    void exportToPdf(const QString& fileName);

    QWidget* chartContainer;
    QComboBox* statTypeComboBox;
    QPushButton* exportButton;
    QPushButton* refreshButton;

    QString currentRequestId;
    int userType;
    int userId;
    QJsonObject currentStats;
    QString currentStatType;
};

#endif // STATISTICSWINDOW_H