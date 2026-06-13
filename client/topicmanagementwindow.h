#ifndef TOPICMANAGEMENTWINDOW_H
#define TOPICMANAGEMENTWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextEdit>

class TopicManagementWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TopicManagementWindow(int userType, int userId, QWidget *parent = nullptr);
    ~TopicManagementWindow();

private slots:
    void onAddTopicClicked();
    void onEditTopicClicked();
    void onDeleteTopicClicked();
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onStatusFilterChanged(int index);
    void onTopicResponseReceived(const QString& requestId, const QJsonObject& response);
    void onAddTopicDialogAccepted();
    void onEditTopicDialogAccepted();

private:
    void initUI();
    void initConnections();
    void loadTopics();
    void updateTopicTable(const QJsonArray& topics);
    void showTopicDialog(const QJsonObject& topicInfo = QJsonObject());
    void loadTeachers();

    QTableWidget* topicTable = nullptr;
    QLineEdit* searchLineEdit = nullptr;
    QComboBox* statusFilterComboBox = nullptr;
    QComboBox* teacherComboBox = nullptr;
    QPushButton* addTopicButton = nullptr;
    QPushButton* editTopicButton = nullptr;
    QPushButton* deleteTopicButton = nullptr;
    QPushButton* refreshButton = nullptr;

    QString currentRequestId;
    QString currentRequestType;
    QDialog* topicDialog = nullptr;
    QLineEdit* topicNameLineEdit = nullptr;
    QComboBox* topicTeacherComboBox = nullptr;
    QTextEdit* descriptionTextEdit = nullptr;
    QTextEdit* requirementsTextEdit = nullptr;
    QLineEdit* maxStudentsLineEdit = nullptr;
    QComboBox* statusComboBox = nullptr;
    
    int userType;
    int userId;
    int teacherId;
    QJsonArray teachers;
};

#endif // TOPICMANAGEMENTWINDOW_H