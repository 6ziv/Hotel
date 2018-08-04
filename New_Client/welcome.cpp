#include "welcome.h"
#include "logindialog.h"
#include"windows.h"
#include<QDebug>
#include"client.h"
Welcome::Welcome(QWidget *parent)
    :QDialog(parent)
{
    Client::connect();
    QSize size=QApplication::desktop()->size();
    this->setGeometry(size.width()/3,size.height()*3/8,size.width()/3,size.height()/4);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_DeleteOnClose,false);
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QPixmap *image=new QPixmap;
    image->load(":/images/welcome.jpg");
    *image=image->scaled(this->size());
    palette.setBrush(QPalette::Window,QBrush(*image));
    this->setPalette(palette);
    delete image;
    timer=new QTimer(this);
    timer->setParent(this);
    timer->setSingleShot(true);
    timer->setInterval(3000);
    timer->start();

    connect(timer,SIGNAL(timeout()),this,SLOT(next()));
    this->show();
}
void Welcome::next(){
    this->accept();
}
Welcome::~Welcome(){
    if(timer)delete timer;
}
