#include "mainwindow.h"
#include"client.h"
#include<QJsonArray>
#include"windows.h"
#include"login.h"
MainWindow *(item::p);
item::item(QWidget *parent, int p_id, QString p_name, QString p_tel)
    :QWidget(parent)
{
    id=p_id;
    this->resize(480,20);
    this->setMinimumSize(480,20);
    this->setMaximumSize(480,20);
    QLabel *lab1=new QLabel(this);
    lab1->setGeometry(0,0,200,20);
    lab1->setText(p_name);
    lab1->show();
    QLabel *lab2=new QLabel(this);
    lab2->setGeometry(200,0,200,20);
    lab2->setText(p_tel);
    lab2->show();
    QPushButton *pb=new QPushButton(this);
    pb->setFlat(true);
    pb->setGeometry(420,0,60,20);
    pb->setText("OK");
    pb->show();
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(validit()));
}
void item::validit(){
    QJsonObject obj;
    obj.insert("action","admvalid");
    obj.insert("id",id);
    Client::Sendmsg(&obj);
    p->refresh();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    item::p=this;
    if(!Client::connect()){
        MessageBoxA(0,"No Connection","No connection",MB_OK);
        exit(0);
    }
    Login *tmp=new Login(this);
    int rtn=tmp->exec();
    delete tmp;
    if(rtn!=QDialog::Accepted){
        MessageBoxA(0,"Login failed","Login failed",MB_OK);
        exit(0);
    }
    this->setGeometry(250,500,500,500);
    lw=new QListWidget(this);
    lw->setGeometry(10,10,480,480);
    this->show();
    refresh();

}
void MainWindow::refresh(){
    lw->clear();
    QJsonObject obj;
    obj.empty();
    obj.insert("action","admgetall");
    Client::Sendmsg(&obj);
    QString str=Client::getReply();
    QByteArray tmp=str.toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(tmp);
    if(!doc.isArray()){
        MessageBoxA(0,"Error","Error",MB_OK);
        return;
    }
    QJsonArray arr=doc.array();
    for(int i=0;i<arr.size();i++){
        QJsonObject tobj=arr.at(i).toObject();
        item *itm=new item(this,tobj.value("id").toInt(-1),tobj.value("name").toString(),tobj.value("tel").toString());
        QListWidgetItem *Item=new QListWidgetItem(lw);
        Item->setSizeHint(QSize(480,20));
        lw->addItem(Item);
        lw->setItemWidget(Item,itm);
    }

}
MainWindow::~MainWindow()
{

}
