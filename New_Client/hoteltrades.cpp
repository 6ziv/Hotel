#include "hoteltrades.h"
#include "mainwindow.h"
#include"QTabWidget"
#include<QApplication>
#include<QDesktopWidget>
#include<QLabel>
#include"client.h"
#include<QJsonObject>
#include<QFont>
#include"windows.h"
#include<QDate>
#include<QInputDialog>
#include<QJsonArray>
TradeItem_hotel::~TradeItem_hotel(){

}
void TradeItem_hotel::cr(){
    QJsonObject obj;
    obj.empty();
    obj.insert("action","confirm_refund");
    obj.insert("tradeid",this->info.id);
    Client::Sendmsg(&obj);
    info.p->refresh();
}
TradeItem_hotel::TradeItem_hotel(QWidget* parent,TradeInfo_hotel in,int type)
    :QWidget(parent)
{
    QDate zero;
    zero.setDate(1970,1,1);
    info=in;
    QSize size=QApplication::desktop()->size();
    this->resize(size.width()/2-20,size.height()/20);
    QLabel *lab1=new QLabel(this);
    lab1->setGeometry(0,0,size.width()/2-70,size.height()/20);
    lab1->setText(QString::number(in.id)+"\t"+zero.addDays(in.start).toString()+" to "+zero.addDays(in.finish).toString()+"\t共"+QString::number(in.price)+"元");
    QPushButton* pb=new QPushButton(this);
    pb->setGeometry(size.width()/2-70,0,50,size.height()/20);
    pb->setText("OK");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(cr()));
}
bool sort_fun(TradeInfo_hotel inf1,TradeInfo_hotel inf2){
    return inf1.start>inf2.start;
}
HotelTrades::HotelTrades(QWidget *parent)
    :QDialog(parent)
{
    QSize size=QApplication::desktop()->size();
    this->setGeometry(size.width()/4,size.height()/6,size.width()/2,size.height()*2/3);
    lw=new QListWidget(this);
    lw->setGeometry(5,25,this->width()-10,this->height()-10);
    this->show();
    this->refresh();
    this->show();
}
void HotelTrades::refresh(){
    QJsonObject obj;
    obj.empty();
    obj.insert("action","alltrades");
    Client::Sendmsg(&obj);
    QByteArray data=Client::getReply().toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    QJsonArray arr=doc.array();
    QSize size=QApplication::desktop()->size();
    trades.clear();

    for(int i=0;i<arr.size();i++){
        int t=arr[i].toObject().value("state").toInt();
        if(t!=12)continue;
        TradeInfo_hotel tmp;
        tmp.id=arr[i].toObject().value("id").toInt();
        tmp.price=arr[i].toObject().value("price").toInt();
        tmp.start=arr[i].toObject().value("start").toInt();
        tmp.finish=arr[i].toObject().value("finish").toInt();
        tmp.p=this;
        trades.push_back(tmp);

    }
    std::sort(trades.begin(),trades.end(),sort_fun);
    lw->clear();
    for(QVector<TradeInfo_hotel>::iterator iter=trades.begin();iter!=trades.end();iter++){
        TradeItem_hotel *itm=new TradeItem_hotel(this,*iter,0);
        QListWidgetItem *Item=new QListWidgetItem(lw);
        Item->setSizeHint(QSize(size.width()/2-45,50));
        lw->addItem(Item);
        lw->setItemWidget(Item,itm);
    }

}

HotelTrades::~HotelTrades()
{

}
