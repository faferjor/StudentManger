/********************************************************************************
** Form generated from reading UI file 'loginwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *titleWidget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *titleLabel;
    QWidget *loginWidget;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QLabel *statusLabel;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *loginButton;
    QPushButton *exitButton;
    QPushButton *registerButton;
    QLabel *copyrightLabel;

    void setupUi(QDialog *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(400, 401);
        LoginWindow->setStyleSheet(QString::fromUtf8("background-color: #f8f9fa;"));
        verticalLayout = new QVBoxLayout(LoginWindow);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        titleWidget = new QWidget(LoginWindow);
        titleWidget->setObjectName("titleWidget");
        horizontalLayout_3 = new QHBoxLayout(titleWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        titleLabel = new QLabel(titleWidget);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font.setPointSize(16);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(titleLabel);


        verticalLayout->addWidget(titleWidget);

        loginWidget = new QWidget(LoginWindow);
        loginWidget->setObjectName("loginWidget");
        loginWidget->setStyleSheet(QString::fromUtf8("background-color: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 12px rgba(0, 0, 0, 0.1);"));
        verticalLayout_2 = new QVBoxLayout(loginWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName("verticalLayout_2");
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName("formLayout");
        usernameLabel = new QLabel(loginWidget);
        usernameLabel->setObjectName("usernameLabel");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font1.setPointSize(12);
        usernameLabel->setFont(font1);

        formLayout->setWidget(0, QFormLayout::LabelRole, usernameLabel);

        usernameLineEdit = new QLineEdit(loginWidget);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setFont(font1);
        usernameLineEdit->setStyleSheet(QString::fromUtf8("border: 1px solid #ced4da; border-radius: 4px; padding: 8px; background-color: #f8f9fa;"));
        usernameLineEdit->setMaxLength(50);

        formLayout->setWidget(0, QFormLayout::FieldRole, usernameLineEdit);

        passwordLabel = new QLabel(loginWidget);
        passwordLabel->setObjectName("passwordLabel");
        passwordLabel->setFont(font1);

        formLayout->setWidget(1, QFormLayout::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(loginWidget);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setFont(font1);
        passwordLineEdit->setStyleSheet(QString::fromUtf8("border: 1px solid #ced4da; border-radius: 4px; padding: 8px; background-color: #f8f9fa;"));
        passwordLineEdit->setMaxLength(20);
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::FieldRole, passwordLineEdit);


        verticalLayout_2->addLayout(formLayout);

        statusLabel = new QLabel(loginWidget);
        statusLabel->setObjectName("statusLabel");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font2.setPointSize(10);
        statusLabel->setFont(font2);
        statusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(statusLabel);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        loginButton = new QPushButton(loginWidget);
        loginButton->setObjectName("loginButton");
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font3.setPointSize(12);
        font3.setBold(true);
        loginButton->setFont(font3);
        loginButton->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #007bff; color: white; border: none; border-radius: 4px; padding: 10px 20px; }\n"
"QPushButton:hover { background-color: #0056b3; }\n"
"QPushButton:pressed { background-color: #004085; }\n"
"QPushButton:disabled { background-color: #6c757d; }"));

        horizontalLayout->addWidget(loginButton);

        exitButton = new QPushButton(loginWidget);
        exitButton->setObjectName("exitButton");
        exitButton->setFont(font3);
        exitButton->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #6c757d; color: white; border: none; border-radius: 4px; padding: 10px 20px; }\n"
"QPushButton:hover { background-color: #545b62; }\n"
"QPushButton:pressed { background-color: #4e555b; }"));

        horizontalLayout->addWidget(exitButton);


        verticalLayout_3->addLayout(horizontalLayout);

        registerButton = new QPushButton(loginWidget);
        registerButton->setObjectName("registerButton");
        QFont font4;
        font4.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font4.setPointSize(11);
        font4.setBold(false);
        registerButton->setFont(font4);
        registerButton->setStyleSheet(QString::fromUtf8("QPushButton { background-color: transparent; color: #007bff; border: none; padding: 5px; text-decoration: underline; }\n"
"QPushButton:hover { color: #0056b3; }"));

        verticalLayout_3->addWidget(registerButton);


        verticalLayout_2->addLayout(verticalLayout_3);


        verticalLayout->addWidget(loginWidget);

        copyrightLabel = new QLabel(LoginWindow);
        copyrightLabel->setObjectName("copyrightLabel");
        QFont font5;
        font5.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font5.setPointSize(9);
        copyrightLabel->setFont(font5);
        copyrightLabel->setStyleSheet(QString::fromUtf8("color: #6c757d;"));
        copyrightLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(copyrightLabel);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QDialog *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "\345\244\247\345\255\246\347\224\237\345\260\217\347\273\204\350\257\276\351\242\230\351\200\211\351\242\230\347\256\241\347\220\206\347\263\273\347\273\237 - \347\231\273\345\275\225", nullptr));
        titleLabel->setText(QCoreApplication::translate("LoginWindow", "\345\244\247\345\255\246\347\224\237\345\260\217\347\273\204\350\257\276\351\242\230\351\200\211\351\242\230\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        usernameLabel->setText(QCoreApplication::translate("LoginWindow", "\350\264\246\345\217\267\357\274\232", nullptr));
        passwordLabel->setText(QCoreApplication::translate("LoginWindow", "\345\257\206\347\240\201\357\274\232", nullptr));
        statusLabel->setText(QString());
        loginButton->setText(QCoreApplication::translate("LoginWindow", "\347\231\273\345\275\225", nullptr));
        exitButton->setText(QCoreApplication::translate("LoginWindow", "\351\200\200\345\207\272", nullptr));
        registerButton->setText(QCoreApplication::translate("LoginWindow", "\346\262\241\346\234\211\350\264\246\345\217\267\357\274\237\347\253\213\345\215\263\346\263\250\345\206\214", nullptr));
        copyrightLabel->setText(QCoreApplication::translate("LoginWindow", "\302\251 2024 \345\244\247\345\255\246\347\224\237\345\260\217\347\273\204\350\257\276\351\242\230\351\200\211\351\242\230\347\256\241\347\220\206\347\263\273\347\273\237 v1.0.0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
