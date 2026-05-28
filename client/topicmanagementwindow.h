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

    QTableWidget* topicTable;
    QLineEdit* searchLineEdit;
    QComboBox* statusFilterComboBox;
    QComboBox* teacherComboBox;
    QPushButton* addTopicButton;
    QPushButton* editTopicButton;
    QPushButton* deleteTopicButton;
    QPushButton* refreshButton;

    QString currentRequestId;
    QDialog* topicDialog;
    QLineEdit* topicNameLineEdit;
    QComboBox* topicTeacherComboBox;
    QTextEdit* descriptionTextEdit;
    QTextEdit* requirementsTextEdit;
    QLineEdit* maxStudentsLineEdit;
    QComboBox* statusComboBox;
    
    int userType;
    int userId;
    QJsonArray teachers;
};

#endif // TOPICMANAGEMENTWINDOW_H