#include "hotel_detail.h"
#include"book.h"
#include"windows.h"
#include "client.h"
#include"mainwindow.h"
#include<QApplication>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QDesktopWidget>
item2::item2(QWidget *parent,RoomSimpInfo &info)
    :QWidget(parent)
{
    b=info;

    QSize size=QApplication::desktop()->size();
    this->resize(size.width()/4-20,size.height()/10);
    this->setMinimumSize(QSize(size.width()/4-20,size.height()/10));

    QLabel* name=new QLabel(this);
    name->setText(info.type);
    name->setGeometry(0,0,size.width()/6-20,20);
    QFont font=name->font();
    font.setPointSize(15);
    name->setFont(font);
    name->show();

    QLabel* Price=new QLabel(this);
    Price->setText("￥"+QString::number(info.price));
    Price->setGeometry(size.width()/6,0,size.width()/6-40,20);
    Price->show();

    this->show();
    this->update();
}
void item2::mouseReleaseEvent(QMouseEvent *event){
    QWidget::mouseReleaseEvent(event);
    Book d(this,b);
    d.exec();
}

bool comp(RoomSimpInfo inf1,RoomSimpInfo inf2){
    return inf1.price<inf2.price;
}

Hotel_Detail::Hotel_Detail(QWidget *parent,HotelSimpInfo info) :
    QDialog(parent)
{

    QSize size=QApplication::desktop()->size();
    this->setGeometry(size.width()/3,size.height()/4,size.width()/3,size.height()/2);
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    QLabel *name=new QLabel(this);
    QLabel *desc=new QLabel(this);
    QLabel *Tel=new QLabel(this);
    QLabel *lowest=new QLabel(this);
    Stars *stars=new Stars(this);
    name->setGeometry(0,0,size.width()/9,25);

    stars->setGeometry(size.width()/9,30,size.width()/9,25);
    stars->setstars(info.stars);
    stars->show();

    QFont font=name->font();
    font.setPointSize(20);
    name->setFont(font);
    name->setText(info.name);
    name->show();

    desc->setGeometry(size.width()/9,0,size.width()*2/9,25);
    font.setPointSize(15);
    desc->setFont(font);
    desc->setText(info.desc);
    desc->show();

    Tel->setGeometry(0,30,size.width()/9,25);
    font.setPointSize(15);
    Tel->setFont(font);
    Tel->setText(info.tel);
    Tel->show();

    lowest->setGeometry(size.width()*2/9,30,size.width()/9,25);
    font.setPointSize(15);
    lowest->setFont(font);
    lowest->setText(info.tel);
    lowest->show();

    QListWidget *rooms=new QListWidget(this);
     rooms->setGeometry(size.width()/100,size.height()*4/21,size.width()/3-size.width()/50,size.height()*2/7);
    QVector<RoomSimpInfo> r_list;
    QJsonObject obj;
    obj.empty();
    obj.insert("action","all_room");
    obj.insert("hotel",info.id);
    Client::Sendmsg(&obj);
    QByteArray data=Client::getReply().toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    if(doc.isArray()){
        r_list.clear();
        QJsonArray arr=doc.array();
        for(int i=0;i<arr.size();i++){
            RoomSimpInfo inf;
            QJsonObject obj=arr.at(i).toObject();

            inf.id=obj.take("id").toInt();
            inf.type=obj.take("type").toString();
            inf.price=obj.take("price").toInt();
            qDebug()<<inf.price;
            r_list.push_back(inf);
        }
        std::sort(r_list.begin(),r_list.end(),comp);
        for(QVector<RoomSimpInfo>::iterator iter=r_list.begin();iter!=r_list.end();iter++){
            item2 *tmp=new item2(this,*iter);
            QListWidgetItem *Item=new QListWidgetItem(rooms);
            Item->setSizeHint(QSize(size.width()/3-40,50));
            rooms->addItem(Item);
            rooms->setItemWidget(Item,tmp);
        }
    }
}

Hotel_Detail::~Hotel_Detail()
{

}
