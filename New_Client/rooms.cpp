#include "rooms.h"
#include"client.h"
#include"windows.h"
RoomItem2::RoomItem2(QWidget *parent,RoomInf info,RoomsP *tp)
    :QWidget(parent)
{
    this->trueparent=tp;
    this->data=info;
    this->resize(480,50);
    this->setMinimumSize(480,50);
    this->setMaximumSize(480,50);
    type=new QLabel(this);
    type->setGeometry(0,0,100,20);
    type->setText(data.type);
    type->show();

    desc=new QLabel(this);
    desc->setGeometry(130,0,350,20);
    desc->setText(data.desc);
    desc->show();

    price=new QLabel(this);
    price->setGeometry(0,30,50,20);
    price->setText(QString::number(data.price));
    price->show();

    number=new QLabel(this);
    number->setGeometry(60,30,50,20);
    number->setText(QString::number(data.number));
    number->show();

    confirm=new QPushButton(this);
    confirm->setFlat(true);
    confirm->setText("确认");
    confirm->setGeometry(120,30,50,20);
    connect(confirm,SIGNAL(clicked(bool)),this,SLOT(save()));
    confirm->hide();

    undo=new QPushButton(this);
    undo->setFlat(true);
    undo->setText("取消");
    undo->setGeometry(170,30,50,20);
    connect(undo,SIGNAL(clicked(bool)),this,SLOT(cancel()));
    undo->hide();

    if(data.id==-1){
        number->setText("");
        price->setText("");
    }
    editit=new QPushButton(this);
    editit->setFlat(true);
    editit->setText("编辑");
    editit->setGeometry(120,30,50,20);
    editit->show();
    connect(editit,SIGNAL(clicked(bool)),this,SLOT(edit()));


    edittype=new QLineEdit(this);
    edittype->setGeometry(type->geometry());
    edittype->setText(type->text());
    edittype->setVisible(false);

    editdesc=new QLineEdit(this);
    editdesc->setGeometry(desc->geometry());
    editdesc->setText(desc->text());
    editdesc->setVisible(false);

    editprice=new QLineEdit(this);
    editprice->setGeometry(price->geometry());
    editprice->setText(price->text());
    editprice->setVisible(false);

    editnumber=new QLineEdit(this);
    editnumber->setGeometry(number->geometry());
    editnumber->setText(number->text());
    editnumber->setVisible(false);

}
void RoomItem2::edit(){
    qDebug()<<"edit";
    if(trueparent->editing==this)return;
    if(trueparent->editing!=0)
        trueparent->editing->cancel();
    type->hide();
    desc->hide();
    price->hide();
    number->hide();
    edittype->setText(type->text());
    editdesc->setText(desc->text());
    editprice->setText(price->text());
    editnumber->setText(number->text());
    edittype->show();
    editdesc->show();
    editprice->show();
    editnumber->show();
    editit->hide();
    undo->show();
    confirm->show();
    trueparent->editing=this;
}
void RoomItem2::cancel(){
    type->show();
    desc->show();
    price->show();
    number->show();
    edittype->setText(type->text());
    editdesc->setText(desc->text());
    editprice->setText(price->text());
    editnumber->setText(number->text());
    edittype->hide();
    editdesc->hide();
    editprice->hide();
    editnumber->hide();
    editit->show();
    undo->hide();
    confirm->hide();
    trueparent->editing=0;
}
void RoomItem2::save(){
    QJsonObject obj;
    if(data.id!=-1)obj.insert("action","chroom");else obj.insert("action","newroom");
    obj.insert("roomid",this->data.id);
    obj.insert("type",edittype->text());
    obj.insert("name",editdesc->text());
    obj.insert("price",editprice->text().toInt());
    obj.insert("number",editnumber->text().toInt());
    Client::Sendmsg(&obj);
    trueparent->refresh();
}
RoomsP::RoomsP(QWidget *parent)
    :QDialog(parent)
{
    list=new QListWidget(this);
    this->setGeometry(250,100,500,350);
    list->setGeometry(5,5,490,340);
    list->show();
    refresh();
}
void RoomsP::refresh(){
    editing=0;
    list->clear();
    QJsonObject obj;
    obj.empty();
    obj.insert("action","all_room");
    int rtn=Client::Sendmsg(&obj);
    if(rtn!=0){
        MessageBoxA(0,"Error","Error",MB_OK);
        this->reject();
    }
    QString str=Client::getReply();
    QByteArray tmp=str.toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(tmp);
    if(!doc.isArray()){
        MessageBoxA(0,"Error","Error",MB_OK);
        this->reject();
    }
    QJsonArray arr=doc.array();
    for(int i=0;i<arr.size();i++){
        QJsonObject obj;
        obj=arr.at(i).toObject();
        RoomInf inf;
        inf.desc=obj.value("desc").toString();
        inf.id=obj.value("id").toInt();
        inf.number=obj.value("number").toInt();
        inf.price=obj.value("price").toInt();
        inf.type=obj.value("type").toString();
        RoomItem2 *itm=new RoomItem2(this,inf,this);
        QListWidgetItem *Item=new QListWidgetItem(list);
        Item->setSizeHint(QSize(480,50));
        list->addItem(Item);
        list->setItemWidget(Item,itm);
    }
    RoomInf inf;
    inf.desc="新建";
    inf.id=-1;
    RoomItem2 *itm=new RoomItem2(this,inf,this);
    QListWidgetItem *Item=new QListWidgetItem(list);
    Item->setSizeHint(QSize(480,50));
    list->addItem(Item);
    list->setItemWidget(Item,itm);

}
