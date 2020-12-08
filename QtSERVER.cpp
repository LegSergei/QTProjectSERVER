#include "QtSERVER.h"
#include <QTcpServer>// Инклюд класса
#include <QTcpSocket>
#include <qdatetime.h>
//
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>



QtSERVER::QtSERVER(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.pb_start, &QPushButton::clicked, this, &QtSERVER::my_Start);// Подключение кнопки запуска сервера
    connect(ui.pb_Exit, &QPushButton::clicked, this, &QtSERVER::Exit);
    tcp_server = new QTcpServer(this); // Создание объекта класса
    connect(tcp_server, &QTcpServer::newConnection, this, &QtSERVER::my_NewConnection);// Подключаем сигнал нового соединения к функции onNewConnection
    // база данных
    QSqlDatabase my_db = QSqlDatabase::addDatabase("QMYSQL", "USERSDB");
    my_db.setHostName("127.0.0.1");
    my_db.setPort(3306);
    my_db.setDatabaseName("users_db");
    my_db.setUserName("Free_FA");
    my_db.setPassword("FFFA51");
    // база данных
    Chat_number = 1;

}



void QtSERVER::my_Start()
{
    QString state = ui.pb_start->text(); // В зависимости от надписанного в кнопке текста определяется текущее состояние сервера
    if (state == u8"Запустить сервер") { // Это случай когда сервер отключен и его надо включить
        bool ok;
        int port = ui.le_port->text().toInt(&ok); // Считываем номер порта из лайн эдита
        if (!ok) { // Защита от невалидного значения порта
            ui.pte_main->appendPlainText(u8"Порт не валиден");
            return;
        }
        ui.l_port->setEnabled(false); //Делаем лейбл порта недоступным (не обязательно)
        ui.le_port->setEnabled(false); //Делаем окно ввода номера порта недоступным
        ui.pb_start->setText(u8"Остановить сервер"); // Меняем надпись на кнопке
        tcp_server->listen(QHostAddress::Any, port); // Начинаем слушать этот порт
        QDateTime date = QDateTime::currentDateTime();
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Сервер запущен");

    }
    else if (state == u8"Остановить сервер") { // А это случай, когда сервер уже работает и его надо остановить

        ui.l_port->setEnabled(true); //Делаем заблокированное доступным
        ui.le_port->setEnabled(true); 
        ui.pb_start->setText(u8"Запустить сервер");
        tcp_server->close();
        QDateTime date = QDateTime::currentDateTime();
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Сервер остановлен");
    }
    



}

void QtSERVER::my_NewConnection()
{
    QTcpSocket* socket = tcp_server->nextPendingConnection(); // Чтобы получить сокет для входящего соединения надо вызвать netPendingConnection
    connect(socket, &QTcpSocket::connected, this, &QtSERVER::my_Connected); //Сигнал об установлении соединения подключен к соотв блоку кода
    connect(socket, &QTcpSocket::disconnected, this, &QtSERVER::my_DConnected);//Сигнал о разрыве соединения подключен к соотв блоку кода
    connect(socket, &QTcpSocket::stateChanged, this, &QtSERVER::my_stateChanged);//Сигнал об изменении состояния соединения подключен к соотв блоку кода
    connect(socket, &QTcpSocket::readyRead, this, &QtSERVER::my_readyRead);//Сигнал о готовности к передачи подключен к соотв блоку кода
//    s = qobject_cast<QTcpSocket*>(sender()); //Сохранение сокета. 
}

void QtSERVER::my_Connected()
{
    QDateTime date = QDateTime::currentDateTime();
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Соединение установлено");
//    s = qobject_cast<QTcpSocket*>(sender()); //Сохранение сокета.
}


void QtSERVER::my_DConnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender()); //Получаем указатель на тот сокет, который сгенерировал сигнал об окончании содинения
    socket->deleteLater(); // Удалит сокет только после того, как он отправит все служебные TCP-пакеты. Важно.
    QDateTime date = QDateTime::currentDateTime();
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Соединение разорвано");
}

void QtSERVER::my_stateChanged(QAbstractSocket::SocketState socketState)
{
    QString state; // Состояние соединения 
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        state = "UnconnectedState";
        break;
    case QAbstractSocket::HostLookupState:
        state = "HostLookupState";
        break;
    case QAbstractSocket::ConnectingState:
        state = "ConnectingState";
        break;
    case QAbstractSocket::ConnectedState:
        state = "ConnectedState";
        break;
    case QAbstractSocket::BoundState:
        state = "BoundState";
        break;
    case QAbstractSocket::ClosingState:
        state = "ClosingState";
        break;
    case QAbstractSocket::ListeningState:
        state = "ListeningState";
        break;
    }
    QDateTime date = QDateTime::currentDateTime();
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Сокет изменил состояние на: " + state);
}

void QtSERVER::my_readyRead()
{
//    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket = qobject_cast<QTcpSocket*>(sender());
    QDateTime date = QDateTime::currentDateTime();
    if (socket == nullptr) {
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": ОШИБКА! Сокет удален.");
        return;
    }
    if (!socket->isValid()) {
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": ОШИБКА! Сокет не валиден.");
        return;
    }
    if (!socket->isOpen()) {
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": ОШИБКА! Сокет закрыт.");
        return;
    }
    QByteArray ba_array = socket->readAll(); // Сюда будем читать из сокета. Есть функция чтения по строчкам для форматированного текста.
    QString client_request = QString(ba_array);
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Сокет сообщил:\n " + client_request);
    //Обработка запроса
    int req_result = Client_request_identification(client_request);
    if (req_result == 1)
    {
        int reg_result = Client_request_registration(client_request);
        //запрос к БД
        if (reg_result == 2)// это если пользователь существует.
        {
            QByteArray ba_array_reg = u8"AARAТакой пользователь уже существует";
            qint64 res = socket->write(ba_array_reg);
        }
        if (reg_result == 3)// это если пользователь существует.
        {
            QByteArray ba_array_reg = u8"AARAРегистрация успешно завершена";
            qint64 res = socket->write(ba_array_reg);
        }
    }

    if (req_result == 2)
    {
        int reg_autho = Client_request_authorization(client_request);
        //запрос к БД
        if (reg_autho == 2)// Пароль совпал
        {
            QByteArray ba_array_aut = u8"AAAAВы успешно авторизировались";
            qint64 res = socket->write(ba_array_aut);
        }
        if (reg_autho == 3)// Пароль не совпал
        {
            QByteArray ba_array_aut = u8"AAAAПароль не совпал";
            qint64 res = socket->write(ba_array_aut);
        }
    }

    if (req_result == 3)
    {
       bool resu = Client_request_send_message(client_request);
        //запрос к БД
    }
    if (req_result == 4)
    {
        Client_request_ALL_in();
        //запрос к БД
    }


}

int QtSERVER::Client_request_identification(QString request)
{
    if (request.startsWith("AA8A")) 
    {
        return 1;// 1 - значит пришел запрос от Регистрации
    }
    if (request.startsWith("AA9A"))
    {
        return 2;// 2 - значит пришел запрос от Войти
    }
    if (request.startsWith("AA0A"))
    {
        return 3;// 3 - значит пришел запрос от Отправить
    }
    if (request.startsWith("AA1A"))
    {
        Chat_number = 1;
        return 4;// 4 - значит надо отгрузить всю базу
    }
    if (request.startsWith("AA2A"))
    {
        Chat_number = 2;
        return 4;// 4 - значит надо отгрузить всю базу
    }

}

int QtSERVER::Client_request_registration(QString request)
{
    QStringList list = request.split(" ");
    // база данных
    QSqlDatabase my_db = QSqlDatabase::database("USERSDB", false); //получаем объект при помощи имени соединения
    if (!my_db.open()) // Открываем БД.
    {
        QDateTime date = QDateTime::currentDateTime();
        ui.pte_main->appendPlainText(my_db.lastError().text());
        return 0;
    }
    QDateTime date = QDateTime::currentDateTime();
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Соединение с базой данных пользователей успешно");
    // база данных
    //Запрос к бд
    QSqlQuery q(my_db);
    q.prepare(
        "SELECT  Login"
        "   FROM user_name un"
        "   WHERE un.Login = :lg"
    );
    q.bindValue(":lg", list[1]);
    if (!q.exec())
    {
        ui.pte_main->appendPlainText(q.lastError().text());
        return 1;
    }
    if (q.next())
    {
        QString str = q.value("Login").toString();
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Ответ от базы данных пользователей получен");
        ui.pte_main->appendPlainText(str);
        ui.pte_main->appendPlainText(u8"Такой пользователь уже есть");
        return 2;
    }
    else
    {
        ui.pte_main->appendPlainText(u8"Такого пользователя не существует"); // тут будем делать INSERT
        q.prepare(
             "INSERT INTO user_name(Login, Password) VALUES(:lg, :ps)"
        );
        q.bindValue(":lg", list[1]);
        q.bindValue(":ps", list[2]);
        if (!q.exec())
        {
            ui.pte_main->appendPlainText(q.lastError().text());
            return 1;
        }
        return 3;
    }
    my_db.close();
}

int QtSERVER::Client_request_authorization(QString request)
{
    QStringList list = request.split(" ");
    QSqlDatabase my_db = QSqlDatabase::database("USERSDB", false);
    if (!my_db.open())
    {
        QDateTime date = QDateTime::currentDateTime();
        ui.pte_main->appendPlainText(my_db.lastError().text());
        return 0;
    }
    QDateTime date = QDateTime::currentDateTime();
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Соединение с базой данных пользователей успешно");
    QSqlQuery q(my_db);
    q.prepare(
        "SELECT  Login, Password"
        "   FROM user_name un"
        "   WHERE un.Login = :lg"    
    );
    q.bindValue(":lg", list[1]);
    if (!q.exec())
    {
        ui.pte_main->appendPlainText(q.lastError().text());
        return 1;
    }
    if (q.next())
    {
        QString Password_string = q.value("Password").toString();
        ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Ответ от базы данных пользователей получен");
        ui.pte_main->appendPlainText(Password_string);
        if (Password_string == list[2])// Проверка совпадает ли значение из бд с введеным пользователем паролем.
        {
            ui.pte_main->appendPlainText(u8"Пароль совпал");
            return 2; //Успех, пароль совпал.
        }
        else
        {
            ui.pte_main->appendPlainText(u8"Пароль не совпал");
            return 3; // Пароль не верен
        }
    }
    else
    {
        ui.pte_main->appendPlainText(u8"Такого пользователя не существует");
        return 4;
    }
    return 0;
    my_db.close();
}

bool QtSERVER::Client_request_send_message(QString request)
{
    QStringList list = request.split("%&?");
    ui.pte_main->appendPlainText(u8"Вошли в ветку обработки сообщений");
    QDateTime date = QDateTime::currentDateTime();
    QString time = date.toString("dd.MM.yyyy hh:mm:ss");
    QSqlDatabase my_db = QSqlDatabase::database("USERSDB", false);
    if (!my_db.open())
    {
        QDateTime date = QDateTime::currentDateTime();
        ui.pte_main->appendPlainText(my_db.lastError().text());
        return 0;
    }
    ui.pte_main->appendPlainText(date.toString("dd.MM.yyyy hh:mm:ss") + u8": Соединение с базой данных пользователей успешно");
    QSqlQuery q(my_db);
    if (Chat_number == 1)
    {
        q.prepare(
            "INSERT INTO users_messages(Name, Date, Message) VALUES(:nm, :dt, :me)"
        );
    }
    if (Chat_number == 2)
    {
        q.prepare(
            "INSERT INTO users_messages_2(Name, Date, Message) VALUES(:nm, :dt, :me)"
        );
    }
    q.bindValue(":nm", list[1]);
    q.bindValue(":dt", time);
    q.bindValue(":me", list[2]);
    if (!q.exec())
    {
        ui.pte_main->appendPlainText(q.lastError().text());
        return 1;
    }
    my_db.close();
    return true;

}

void QtSERVER::Client_request_ALL_in()
{
    QSqlDatabase my_db = QSqlDatabase::database("USERSDB", false);
    if (!my_db.open())
    {
        QDateTime date = QDateTime::currentDateTime();
        ui.pte_main->appendPlainText(my_db.lastError().text());
        return;
    }
    QSqlQuery q(my_db);
    if (Chat_number == 1)
    {
        q.prepare(
            "SELECT *"
            "FROM users_messages um;"
        );
    }
    if (Chat_number == 2)
    {
        q.prepare(
            "SELECT *"
            "FROM users_messages_2 um;"
        );
    }
    if (!q.exec())
    {
        ui.pte_main->appendPlainText(q.lastError().text());
        return;
    }
//    QString result = u8"AA3A";
    while (q.next())
    {
        QString nm = q.value("Name").toString();
        QString dt = q.value("Date").toString();
        QString ms = q.value("Message").toString();
        QString result = u8"AA3A" + nm + u8" " + dt + u8" : " + ms + u8"$%&";
        ui.pte_main->appendPlainText(result);
        QByteArray ba_buffer;
        ba_buffer.append(result);
        qint64 res = socket->write(ba_buffer);
    }
 
    
}

void QtSERVER::Exit()
{
    QApplication::quit();
}
