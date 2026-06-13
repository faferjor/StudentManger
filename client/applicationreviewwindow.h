#ifndef APPLICATIONREVIEWWINDOW_H
#define APPLICATIONREVIEWWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>

class ApplicationReviewWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ApplicationReviewWindow(int userType, int userId, QWidget *parent = nullptr);
    ~ApplicationReviewWindow();

private slots:
    void onApproveClicked();
    void onRejectClicked();
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onStatusFilterChanged(int index);
    void onTopicFilterChanged(int index);
    void onResponseReceived(const QString& requestId, const QJsonObject& response);
    void showApplicationDetail(const QJsonObject& appInfo);
    void showRejectDialog(const QJsonObject& appInfo);

private:
    void initUI();
    void initConnections();
    void loadApplications();
    void loadTopics();
    void updateApplicationTable(const QJsonArray& applications);
    void updateApplicationStatus(int applicationId, int status, const QString& rejectReason = "");

    QTableWidget* applicationTable = nullptr;
    QLineEdit* searchLineEdit = nullptr;
    QComboBox* statusFilterComboBox = nullptr;
    QComboBox* topicFilterComboBox = nullptr;
    QPushButton* approveButton = nullptr;
    QPushButton* rejectButton = nullptr;
    QPushButton* refreshButton = nullptr;

    QString currentRequestId;
    QString currentRequestType;
    int userType;
    int userId;
    int teacherId = 0;
    int groupId = 0;
    QJsonArray topics;
};

#endif // APPLICATIONREVIEWWINDOW_H