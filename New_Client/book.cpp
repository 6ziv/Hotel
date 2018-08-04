#include "book.h"
#include<QBuffer>
#include"client.h"
#include"windows.h"
void Book::change(){
    QDate zero;
    zero.setDate(1970,1,1);
    if(date1->date()>date2->date())date2->setDate(date1->date());
    int d1=zero.daysTo(date1->date());
    int d2=zero.daysTo(date2->date());
    int diff=d2-d1+1;
    total->setText(QString::number(diff*info.price));
    number->setText(QString::number(diff));
}
Book::Book(QWidget *parent,RoomSimpInfo in)
    :QDialog(parent)
{
    info.id=in.id;
    info.price=in.price;
    info.type=in.type;
    this->resize(400,400);
    QLabel *name=new QLabel(this);
    name->setGeometry(10,20,50,20);
    name->setText("房型："+in.type);

    QLabel *dat1=new QLabel(this);
    QLabel *dat2=new QLabel(this);
    dat1->setGeometry(10,60,50,20);
    dat1->setText("入住时间");
    dat2->setGeometry(210,60,50,20);
    dat2->setText("离店时间");
    date1=new QDateEdit(this);
    date1->setGeometry(70,60,100,20);

    date1->setDate(QDate::currentDate());
    date2=new QDateEdit(this);
    date2->setGeometry(270,60,100,20);
    date2->setDate(QDate::currentDate());

    QLineEdit *price=new QLineEdit(this);
    price->setGeometry(10,100,50,20);
    price->setText(QString::number(info.price));
    price->setEnabled(false);
    QLabel *multi=new QLabel(this);
    multi->setGeometry(60,100,20,20);
    QFont font=multi->font();
    font.setPointSize(20);
    multi->setFont(font);
    multi->setText("x");
    number=new QLineEdit(this);
    number->setGeometry(80,100,50,20);
    number->setText("1");
    number->setEnabled(false);
    QLabel *equal=new QLabel(this);
    equal->setGeometry(130,100,20,20);
    font=equal->font();
    font.setPointSize(20);
    equal->setFont(font);
    equal->setText("=");
    total=new QLineEdit(this);
    total->setGeometry(150,100,50,20);
    total->setText(QString::number(info.price));
    total->setEnabled(false);
    QPushButton* submit=new QPushButton(this);
    submit->setGeometry(220,100,50,20);
    submit->setText("提交");
    connect(submit,SIGNAL(pressed()),this,SLOT(go()));
    connect(date1,SIGNAL(dateChanged(QDate)),this,SLOT(change()));
    connect(date2,SIGNAL(dateChanged(QDate)),this,SLOT(change()));

    this->show();
}
void Book::go(){
    QJsonObject obj;
    obj.empty();
    obj.insert("action","book");
    obj.insert("id",info.id);
    QDate zero;
    zero.setDate(1970,1,1);

    int d1=zero.daysTo(date1->date());
    int d2=zero.daysTo(date2->date());

    obj.insert("start",d1);

    obj.insert("finish",d2);

    qDebug()<<Client::Sendmsg(&obj);
    QByteArray arr=Client::getReply().toLatin1();
    arr=QByteArray::fromBase64(arr);
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    QPixmap imageresult;
    qDebug()<<imageresult.loadFromData(arr);
    imageresult=imageresult.scaled(256,256);
    QLabel *lab=new QLabel;

    lab->setGeometry(0,0,256,256);
    lab->setPixmap(imageresult);
    lab->show();
    int old=time(0);
    QJsonObject tobj;
    tobj.empty();
    tobj.insert("action","check_pay");
    while(time(0)<old+1800){
        int rtn=Client::Sendmsg(&tobj);
        if(rtn==0){
            lab->hide();
            return;
        }
        if(rtn==-99){
            lab->hide();
            MessageBoxA(0,"Unknown Error!","Error!",MB_OK);
            return;
        }
        Sleep(3000);
    }
}
