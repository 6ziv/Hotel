#ifndef SERVERLET_H
#define SERVERLET_H
#include "common.h"
#include <QTimer>
#include <QQueue>
#include<QDate>
class Code{
public:
    int id;
    int time;
};
class Serverlet: public QObject
{
    Q_OBJECT
private slots:
    void Server_New_Connect();
    void timeout();
private:
    QTcpServer *Server;
    int clients;
    QQueue<Code> WaitingList;
    QTimer *timer;
    QDate tmpdate;
public:
    Serverlet();
    ~Serverlet();
    QList<QString> Cities;
};

#endif // SERVERLET_H
