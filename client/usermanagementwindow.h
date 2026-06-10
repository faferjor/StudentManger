#ifndef USERMANAGEMENTWINDOW_H
#define USERMANAGEMENTWINDOW_H

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
#include <QGroupBox>

class UserManagementWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UserManagementWindow(QWidget *parent = nullptr);
    ~UserManagementWindow();

private slots:
    void onAddUserClicked();
    void onEditUserClicked();
    void onDeleteUserClicked();
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onUserTypeFilterChanged(int index);
    void onUserResponseReceived(const QString& requestId, const QJsonObject& response);
    void onAddUserDialogAccepted();
    void onEditUserDialogAccepted();

private:
    void initUI();
    void initConnections();
    void loadUsers();
    void updateUserTable(const QJsonArray& users);
    void showUserDialog(const QJsonObject& userInfo = QJsonObject());

    QTableWidget* userTable;
    QLineEdit* searchLineEdit;
    QComboBox* userTypeFilterComboBox;
    QPushButton* addUserButton;
    QPushButton* editUserButton;
    QPushButton* deleteUserButton;
    QPushButton* refreshButton;

    QString currentRequestId;
    QString currentRequestType;  // 新增：保存当前请求的类型
    QDialog* userDialog;
    QLineEdit* usernameLineEdit;
    QLineEdit* passwordLineEdit;
    QLineEdit* realNameLineEdit;
    QLineEdit* emailLineEdit;
    QLineEdit* phoneLineEdit;
    QComboBox* userTypeComboBox;
    QComboBox* statusComboBox;
    QGroupBox* extendGroup;
    
    // 教师和学生小组扩展字段
    QLineEdit* departmentLineEdit;
    QLineEdit* titleLineEdit;
    QTextEdit* researchAreaTextEdit;
    QLineEdit* leaderNameLineEdit;
    QLineEdit* memberCountLineEdit;
    QTextEdit* membersTextEdit;
    QLineEdit* majorLineEdit;
    QLineEdit* gradeLineEdit;
    QLineEdit* classNameLineEdit;
    
    int editingUserId;
};

#endif // USERMANAGEMENTWINDOW_H
