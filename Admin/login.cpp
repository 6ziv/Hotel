#include "login.h"
#include"client.h"
#include"QJsonObject"
Login::Login(QWidget *parent)
    :QDialog(parent)
{
    this->setGeometry(400,350,200,100);
    uname=new QLineEdit(this);
    uname->setGeometry(30,20,150,20);
    pass=new QLineEdit(this);
    pass->setGeometry(30,50,150,20);
    pass->setEchoMode(QLineEdit::Password);

    QPushButton *pb=new QPushButton(this);
    pb->setGeometry(70,75,60,20);
    pb->setText("OK");
    pb->show();
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(dologin()));
}
void Login::dologin(){
    QJsonObject obj;
    obj.insert("action","admlogin");
    obj.insert("username",uname->text());
    obj.insert("password",pass->text());
    int r=Client::Sendmsg(&obj);
    if(r==0)this->accept();else this->reject();
}
