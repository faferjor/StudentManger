#ifndef TOPICBROWSINGWINDOW_H
#define TOPICBROWSINGWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextEdit>

class TopicBrowsingWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TopicBrowsingWindow(int userId, int userType, QWidget *parent = nullptr);
    ~TopicBrowsingWindow();

private slots:
    void onApplyTopicClicked();
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onStatusFilterChanged(int index);
    void onTeacherFilterChanged(int index);
    void onTopicResponseReceived(const QString& requestId, const QJsonObject& response);
    void onApplyDialogAccepted();
    void showTopicDetail(const QJsonObject& topicInfo);

private:
    void initUI();
    void initConnections();
    void loadTopics();
    void loadTeachers();
    void updateTopicTable(const QJsonArray& topics);
    void loadMyApplications();
    void updateAppliedTopics();

    QTableWidget* topicTable = nullptr;
    QLineEdit* searchLineEdit = nullptr;
    QComboBox* statusFilterComboBox = nullptr;
    QComboBox* teacherComboBox = nullptr;
    QPushButton* applyTopicButton = nullptr;
    QPushButton* refreshButton = nullptr;

    QString currentRequestId;
    QString currentRequestType;
    QDialog* applyDialog = nullptr;
    QTextEdit* reasonTextEdit = nullptr;

    int userId;
    int userType;
    int groupId = 0;
    bool teachersLoaded = false;
    bool applicationsLoaded = false;
    QJsonArray myApplications;
    QSet<int> appliedTopicIds;
};

#endif // TOPICBROWSINGWINDOW_H