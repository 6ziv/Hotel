#define QT_NO_DEBUG
#include<QEventLoop>
#include "serverlet.h"
#include "dbmanager.h"
#include "session.h"
#include"time.h"
#include<QDate>
void Serverlet::timeout(){
//    qDebug()<<"timer!";
    if(QDate::currentDate()!=tmpdate)
    {
        tmpdate=QDate::currentDate();
        QDate zero;
        zero.setDate(1970,1,1);
        int now=zero.daysTo(tmpdate);
        DBManager::GoodMorning(now);
    }

    if(WaitingList.size()==0)return;
    while((WaitingList.size()>0)&&(WaitingList.head().time<=time(0))){
        DBManager::Timeout(WaitingList.dequeue().id);
    }

}
Serverlet::Serverlet()
    : QObject()
{
    srand(time(0));
    tmpdate=QDate::currentDate();


    QEventLoop e;
    timer=new QTimer(0);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->start();
    Cities.clear();
    Cities.push_back("City1");
    Cities.push_back("City2");
    Cities.push_back("City3");
    Cities.push_back("City4");
    Cities.push_back("City5");
    Cities.push_back("City6");

    Session::serverlet=this;
    DBManager::init();

    {
        QDate zero;
        zero.setDate(1970,1,1);
        int now=zero.daysTo(tmpdate);
        DBManager::GoodMorning(now);
    }
    Server = new QTcpServer();
    std::cout<<"Binding Port 518...";
    if(Server->listen(QHostAddress::Any, 518))
        std::cout<<"Done!"<<std::endl;
    else{
        std::cout<<"Error!"<<std::endl;
        std::cout<<Server->errorString().toStdString()<<std::endl;
    }
    connect(Server,SIGNAL(newConnection()),this,SLOT(Server_New_Connect()));

    e.exec();
    std::cout<<"ExecDone!";
}

Serverlet::~Serverlet()
{
    std::cout<<"Bye~"<<std::endl;
}

void Serverlet::Server_New_Connect()
{
    //获取客户端连接
    QTcpSocket *s;
    try{
        s=Server->nextPendingConnection();
    }catch(...){
        return;
    }
    try{
        new Session(s);
    }catch(...){
        try{
            s->close();
        }catch(...){
        }
    }
}
