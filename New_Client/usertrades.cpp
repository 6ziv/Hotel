#include "usertrades.h"
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
TradeItem_user::~TradeItem_user(){

}
void TradeItem_user::star(){
    if(info.star!=0)return;
    int rtn=QInputDialog::getInt(0,"评级","请为此订单打分(1-5)",3,1,5);
    QJsonObject obj;
    obj.empty();
    obj.insert("action","review");
    obj.insert("tradeid",info.id);
    obj.insert("star",rtn);
    int rtn1=Client::Sendmsg(&obj);
    if(rtn1){MessageBoxA(0,"Error!","error",MB_OK);return;}
    info.star=rtn;
    stars->setstars((qreal)rtn);
    info.p->refresh();
}
void TradeItem_user::rf(){
    QJsonObject obj;
    obj.empty();
    obj.insert("action","refund");
    obj.insert("tradeid",info.id);
    int rtn=Client::Sendmsg(&obj);
    if(rtn){MessageBoxA(0,"Error!","error",MB_OK);return;}
    info.p->refresh();
}
TradeItem_user::TradeItem_user(QWidget* parent,TradeInfo_user in,int type)
    :QWidget(parent)
{
    info=in;
    QSize size=QApplication::desktop()->size();
    this->resize(size.width()/2-20,size.height()/20);
    QLabel* lab1=new QLabel(this);
    QFont font=lab1->font();
    font.setPointSize(20);
    lab1->setFont(font);
    lab1->setGeometry(10,5,180,25);
    lab1->setText(info.displayname);

    QLabel* lab2=new QLabel(this);
    font.setPointSize(10);
    lab2->setFont(font);
    lab2->setGeometry(200,8,75,20);
    lab2->setText(QDate(1970,1,1).addDays(info.start).toString());

    QLabel *lab3=new QLabel(this);
    font.setPointSize(10);
    lab3->setFont(font);
    lab3->setGeometry(300,8,75,20);
    lab3->setText(QDate(1970,1,1).addDays(info.finish).toString());

    if(type==0){
        stars=new Stars(this);
        stars->setGeometry(400,8,100,20);
        stars->setstars((qreal)in.star);
        connect(stars,SIGNAL(clicked()),this,SLOT(star()));
        stars->show();
    }
    else if (type==1){
        pb=new QPushButton(this);
        font.setPointSize(10);
        pb->setFont(font);
        pb->setGeometry(400,5,100,25);
        pb->setText("申请退款");
        connect(pb,SIGNAL(clicked(bool)),this,SLOT(rf()));
        pb->show();
    }
    this->show();
}
bool sort_fun(TradeInfo_user inf1,TradeInfo_user inf2){
    return inf1.start>inf2.start;
}
User_Trade_List::~User_Trade_List(){
}
User_Trade_List::User_Trade_List(QWidget *parent,int p_type)
    :QWidget(parent)
{
    QSize size=QApplication::desktop()->size();
    this->setGeometry(size.width()/4,size.height()/6,size.width()/2,size.height()*2/3);
    lw=new QListWidget(this);
    lw->setGeometry(5,25,this->width()-10,this->height()-30);
    this->show();
}
UserTrades::UserTrades(QWidget *parent)
    :QDialog(parent)
{
    QSize size=QApplication::desktop()->size();
    QTabWidget *tab=new QTabWidget(this);
    tab->resize(this->size());
    page1=new User_Trade_List(0,0);
    tab->addTab(page1,"已完成");
    page2=new User_Trade_List(0,1);
    tab->addTab(page2,"未完成");
    this->setGeometry(size.width()/4-5,size.height()/6-5,size.width()/2+10,size.height()*2/3+10);
    this->refresh();
    this->show();
}
void UserTrades::refresh(){
    QJsonObject obj;
    obj.empty();
    obj.insert("action","alltrades");
    Client::Sendmsg(&obj);
    QByteArray data=Client::getReply().toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    QJsonArray arr=doc.array();
    QSize size=QApplication::desktop()->size();
    page1->trades.clear();
    page2->trades.clear();
    for(int i=0;i<arr.size();i++){
        int t=arr[i].toObject().value("state").toInt();
        if((t!=11)&&(t!=15))continue;
        TradeInfo_user tmp;
        tmp.displayname=arr[i].toObject().value("desc").toString();
        tmp.id=arr[i].toObject().value("id").toInt();
        tmp.price=arr[i].toObject().value("price").toInt();
        tmp.star=arr[i].toObject().value("star").toInt();
        qDebug()<<tmp.star;
        tmp.start=arr[i].toObject().value("start").toInt();
        tmp.finish=arr[i].toObject().value("finish").toInt();
        tmp.p=this;
        if(t==11)page2->trades.push_back(tmp);
        else page1->trades.push_back(tmp);
    }
    std::sort(page1->trades.begin(),page1->trades.end(),sort_fun);
    std::sort(page2->trades.begin(),page2->trades.end(),sort_fun);
    page1->lw->clear();
    page2->lw->clear();
    for(QVector<TradeInfo_user>::iterator iter=page1->trades.begin();iter!=page1->trades.end();iter++){
        TradeItem_user *itm=new TradeItem_user(page1,*iter,0);
        QListWidgetItem *Item=new QListWidgetItem(page1->lw);
        Item->setSizeHint(QSize(size.width()/2-45,50));
        page1->lw->addItem(Item);
        page1->lw->setItemWidget(Item,itm);
    }
    for(QVector<TradeInfo_user>::iterator iter=page2->trades.begin();iter!=page2->trades.end();iter++){
        TradeItem_user *itm=new TradeItem_user(page2,*iter,1);
        QListWidgetItem *Item=new QListWidgetItem(page2->lw);
        Item->setSizeHint(QSize(size.width()/2-45,50));
        page2->lw->addItem(Item);
        page2->lw->setItemWidget(Item,itm);
    }
}

UserTrades::~UserTrades()
{

}
