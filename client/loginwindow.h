#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QTimer>
#include "networkmanager.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccess(int userId, int userType, const QString& username, const QString& realName);
    void loginFailed(const QString& reason);

private slots:
    void onLoginButtonClicked();
    void onExitButtonClicked();
    void onConnectionStateChanged(NetworkManager::ConnectionState state);
    void onResponseReceived(const QString& requestId, const QJsonObject& response);
    void onErrorOccurred(NetworkManager::ErrorType errorType, const QString& errorMsg);
    void clearStatus();

    void on_loginButton_clicked();

private:
    void initUI();
    void initConnections();
    void showStatus(const QString& message, bool isError = false);
    bool validateInput();
    void resetLoginForm();
    void startLoginRequest();
    void stopLoginRequest();

    Ui::LoginWindow *ui;
    NetworkManager& networkManager;
    QString currentRequestId;
    int loginAttempts;
    QTimer* statusClearTimer;
};

#endif // LOGINWINDOW_H
