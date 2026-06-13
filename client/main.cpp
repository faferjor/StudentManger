#include "mainwindow.h"
#include "loginwindow.h"
#include "networkmanager.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("StudentTopicSystemClient");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("EducationTech");
    QApplication::setOrganizationDomain("example.com");

    // 创建登录窗口
    LoginWindow loginWindow;
    
    // 主窗口指针
    MainWindow* mainWindow = nullptr;
    
    // 处理登录成功信号
    QObject::connect(&loginWindow, &LoginWindow::loginSuccess, 
                     [&](int userId, int userType, const QString& username, const QString& realName) {
        qDebug() << "loginSuccess lambda called";
        // //测试
        // // 不要创建 MainWindow
        // // 只是关闭登录窗口
        // loginWindow.close();

     //   ==========
        qDebug() << "Login success - User:" << username << "(ID:" << userId << ", Type:" << userType << ")";
        
        // 创建主窗口

        qDebug() << "loginSuccess signal received! Creating main window...";

        mainWindow = new MainWindow(userId, userType, username, realName);
        qDebug() << "MainWindow created, showing...";

        mainWindow->show();
        
        // 关闭登录窗口
        qDebug() << "MainWindow shown, closing login window...";
        loginWindow.close();
    });
    
    // 处理登录失败信号
    QObject::connect(&loginWindow, &LoginWindow::loginFailed, 
                     [&](const QString& reason) {
        qDebug() << "Login failed:" << reason;
        QMessageBox::warning(&loginWindow, QObject::tr("登录失败"), reason);
    });
    
    // 显示登录窗口
    loginWindow.show();
    
    // 应用程序退出时清理资源
    QObject::connect(&a, &QApplication::aboutToQuit, [&]() {
        if (mainWindow) {
            delete mainWindow;
        }
        NetworkManager::getInstance().disconnectFromServer();
    });

    return a.exec();
}
