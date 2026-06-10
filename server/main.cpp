#include <QCoreApplication>
#include <QTcpServer>
#include <QThread>
#include <QDebug>
#include "databasehandler.h"
#include "clienthandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 初始化数据库连接（在主线程）
    DatabaseHandler* dbHandler = DatabaseHandler::getInstance();
    if (!dbHandler->connectToDatabase("localhost", "topic_management_system", "root", "123456")) {
        qCritical() << "Failed to connect to database. Server will exit.";
        return 1;
    }

    QTcpServer server;
    quint16 port = 12345;
    if (argc > 1) {
        bool ok;
        quint16 customPort = QString(argv[1]).toUShort(&ok);
        if (ok) port = customPort;
    }

    if (!server.listen(QHostAddress::Any, port)) {
        qCritical() << "Failed to start server:" << server.errorString();
        return 1;
    }

    qInfo() << "Server started on port" << port;

    // QObject::connect(&server, &QTcpServer::newConnection, [&server]() {
    //     QTcpSocket *clientSocket = server.nextPendingConnection();
    //     qintptr socketDescriptor = clientSocket->socketDescriptor();
    //     clientSocket->deleteLater();  // 释放，描述符交给工作线程

    //     QThread *thread = new QThread();
    //     ClientHandler *handler = new ClientHandler(socketDescriptor);
    //     handler->moveToThread(thread);

    //     QObject::connect(handler, &ClientHandler::finished, thread, &QThread::quit);
    //     QObject::connect(thread, &QThread::finished, handler, &ClientHandler::deleteLater);
    //     QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    //     thread->start();
    // });

    //测试
    // QObject::connect(&server, &QTcpServer::newConnection, [&server]() {
    //     QTcpSocket *clientSocket = server.nextPendingConnection();
    //     ClientHandler *handler = new ClientHandler(clientSocket);
    //     // 如果需要多线程，可以创建 QThread 并将 handler 移入线程，但此时 socket 已经在 handler 内部，且已经在正确的线程（当前线程），moveToThread 仍可工作
    //     QThread *thread = new QThread();
    //     handler->moveToThread(thread);
    //     ClientHandler::connect(handler, &ClientHandler::finished, thread, &QThread::quit);
    //     ClientHandler::connect(thread, &QThread::finished, handler, &ClientHandler::deleteLater);
    //     ClientHandler::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    //     thread->start();
    // });


    // 单线程处理新连接：直接创建 ClientHandler，不移动线程
    QObject::connect(&server, &QTcpServer::newConnection, [&server]() {
        QTcpSocket *clientSocket = server.nextPendingConnection();
        qDebug() << "New connection from" << clientSocket->peerAddress().toString();
        ClientHandler *handler = new ClientHandler(clientSocket);
        // 当连接断开时，自动删除 handler
        QObject::connect(handler, &ClientHandler::finished, handler, &ClientHandler::deleteLater);
    });


    return a.exec();
}
