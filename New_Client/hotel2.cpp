#include "hotel2.h"
#include"hoteltrades.h"
#include"windows.h"
#include"chpwd.h"
#include"password.h"
#include"QCryptographicHash"
#include<client.h>
Hotel2::Hotel2(QWidget *parent)
    :QDialog(parent)
{
    resize(490, 209);
    Lname = new QLabel(this);
    Lname->setGeometry(QRect(20, 20, 41, 16));
    Lname->setText("名称");
    name = new QPushButton(this);
    name->setGeometry(QRect(50, 20, 141, 23));
    name->setFlat(true);


    chdesc = new QPushButton(this);
    chdesc->setGeometry(QRect(420, 60, 51, 23));
    chdesc->setVisible(false);
    chdesc->setText("保存");

    tel = new QPushButton(this);
    tel->setGeometry(QRect(320, 20, 161, 23));
    tel->setFlat(true);

    Ltel = new QLabel(this);
    Ltel->setGeometry(QRect(270, 20, 41, 16));
    Ltel->setText("电话");


    Lname = new QLabel(this);
    Lname->setGeometry(QRect(20, 20, 41, 16));
    Lname->setText("名称");

    editdesc = new QLineEdit(this);
    editdesc->setGeometry(QRect(70, 60, 341, 20));
    editdesc->setVisible(false);

    edittel = new QLineEdit(this);
    edittel->setGeometry(QRect(310, 20, 113, 20));
    edittel->setVisible(false);

    chtel = new QPushButton(this);
    chtel->setGeometry(QRect(430, 20, 51, 23));
    chtel->setVisible(false);
    chtel->setText("保存");

    Ldesc = new QLabel(this);
    Ldesc->setGeometry(QRect(20, 60, 41, 16));
    Ldesc->setText("描述");

    desc = new QPushButton(this);
    desc->setGeometry(QRect(70, 60, 401, 23));
    desc->setFlat(true);

    chpwd = new QPushButton(this);
    chpwd->setGeometry(QRect(30, 120, 101, 51));
    chpwd->setText("更改密码");

    Orders = new QPushButton(this);
    Orders->setGeometry(QRect(190, 120, 101, 51));
    Orders->setText("退款请求");

    Rooms = new QPushButton(this);
    Rooms->setGeometry(QRect(350, 120, 101, 51));
    Rooms->setText("房间信息");

    connect(desc,SIGNAL(clicked(bool)),this,SLOT(cdesc()));
    connect(tel,SIGNAL(clicked(bool)),this,SLOT(ctel()));
    connect(chdesc,SIGNAL(clicked(bool)),this,SLOT(do_desc()));
    connect(chtel,SIGNAL(clicked(bool)),this,SLOT(do_tel()));
    connect(chpwd,SIGNAL(clicked(bool)),this,SLOT(cp()));
    connect(Rooms,SIGNAL(clicked(bool)),this,SLOT(r()));
    connect(Orders,SIGNAL(clicked(bool)),this,SLOT(t()));
    refresh();
}
void Hotel2::ctel(){
    editdesc->setVisible(false);
    chdesc->setVisible(false);
    desc->setVisible(true);

    edittel->setVisible(true);
    chtel->setVisible(true);
    tel->setVisible(false);

    edittel->setText(tel->text());
}
void Hotel2::cdesc(){
    editdesc->setVisible(true);
    chdesc->setVisible(true);
    desc->setVisible(false);

    edittel->setVisible(false);
    chtel->setVisible(false);
    tel->setVisible(true);

    editdesc->setText(desc->text());
}

void Hotel2::refresh(){
    QJsonObject obj;
    obj.insert("action","mydetail");
    Client::Sendmsg(&obj);
    QString str=Client::getReply();
    QByteArray json=str.toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(json);
    if(!doc.isObject()){
        MessageBoxA(0,"Error","Error",MB_OK);
        return;
    }
    obj.empty();
    obj=doc.object();
    strname=obj.value("name").toString();
    strtel=obj.value("tel").toString();
    strdesc=obj.value("desc").toString();
    name->setText(strname);
    tel->setText(strtel);
    desc->setText(strdesc);
    chdesc->setVisible(false);
    editdesc->setVisible(false);
    chtel->setVisible(false);
    edittel->setVisible(false);
    desc->setVisible(true);
    tel->setVisible(true);
}
void Hotel2::do_tel(){
    QJsonObject obj;
    obj.insert("action","changetel");
    obj.insert("tel",edittel->text());
    int rtn=Client::Sendmsg(&obj);
    if(rtn<0){
        MessageBoxA(0,"Error","Error",MB_OK);
        return;
    }
    refresh();
}
void Hotel2::do_desc(){
    QJsonObject obj;
    obj.insert("action","changedescription");
    obj.insert("desc",editdesc->text());
    int rtn=Client::Sendmsg(&obj);
    if(rtn<0){
        MessageBoxA(0,"Error","Error",MB_OK);
        return;
    }
    refresh();
}
void Hotel2::cp(){
    QString rtn;
    ChPwd *cpw=new ChPwd(this,&rtn);
    int rrr=cpw->exec();
    delete cpw;
    qDebug()<<rtn;
    cpw=0;
    if(rrr!=QDialog::Accepted)return;
    QByteArray passRaw=rtn.toLatin1();
    QByteArray passCip=QCryptographicHash::hash(passRaw,QCryptographicHash::Sha3_512);
    _512 key;
    key.FromRaw(passCip);
    QJsonObject obj;
    obj.insert("action","chkey");
    obj.insert("pass",(QString)((QByteArray)key));
    Client::Sendmsg(&obj);
    return;
}
void Hotel2::r(){
    RoomsP *tmp=new RoomsP(this);
    tmp->exec();
    delete tmp;

}
void Hotel2::t(){
    HotelTrades *tmp=new HotelTrades(this);
    tmp->exec();
    delete tmp;
}
