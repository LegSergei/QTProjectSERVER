#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtSERVER.h"
#include <QTcpSocket>

class QTcpServer; // Объявление класса
class QtSERVER : public QWidget
{
    Q_OBJECT

public:
    QtSERVER(QWidget *parent = Q_NULLPTR);

private:
    Ui::QtSERVERClass ui;
    QTcpServer* tcp_server; // Указатель на класс
    QTcpSocket* s;
    QTcpSocket* socket;
    int Chat_number;

private slots: // Обработчик нажатия на кнопку
    void my_Start(); // Нажатие на кнопку запуска сервера
    void my_NewConnection(); // Этот слот будет обрабатывать сигнал, возникающий при подключении к серверу
    void my_Connected();// Слот обработки сигнала что подключение состоялось
    void my_DConnected();// Слот обработки сигнала что подключение закрылось
    void my_stateChanged(QAbstractSocket::SocketState socketState); //Обрабатываем сигнал об изменении состояния подключения к серверу
    void my_readyRead();
    int Client_request_identification(QString request);
    int Client_request_registration(QString request);
    int Client_request_authorization(QString request);
    bool Client_request_send_message(QString request);
    void Client_request_ALL_in();
    void Exit();
    void Log(QString str);
};
