#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int userId, int userType, const QString& username, const QString& realName, QWidget *parent = nullptr);
    ~MainWindow();

enum UserType {
        Admin = 1,
        Teacher = 2,
        StudentGroup = 3
    };

private slots:
    void onLogoutClicked();
    void onHomeClicked();
    void onUserManagementClicked();
    void onTopicManagementClicked();
    void onApplyManagementClicked();
    void onStatisticsClicked();
    void onProfileClicked();
    void onConnectionStateChanged(int state);
    void onResponseReceived(const QString& requestId, const QJsonObject& response);
    void updateStatusBar();

private:
    void initUI();
    void initMenu();
    void initStatusBar();
    void initConnections();
    void loadUserInterface();
    void showHomePage();
    void showUserManagementPage();
    void showTopicManagementPage();
    void showApplicationReviewPage();
    void showStatisticsPage();
    void showStudentPage();
    void showTeacherPage();
    void showProfilePage();
    void createAdminInterface();
    void createTeacherInterface();
    void createStudentInterface();

    Ui::MainWindow *ui;
    int userId;
    int userType;
    QString username;
    QString realName;
    QStackedWidget* mainStackedWidget;
    QLabel* statusConnectionLabel;
    QLabel* statusUserLabel;
    QLabel* statusTimeLabel;
};
#endif // MAINWINDOW_H
