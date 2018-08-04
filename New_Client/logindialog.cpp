#include "logindialog.h"
#include"hotel2.h"
#include<QPalette>
#include<windows.h>
#include<QMessageBox>
#include<QIcon>
#include<QComboBox>
#include<QDebug>
#include<QInputDialog>
#include<QCryptographicHash>
#include<QDoubleValidator>
#include"password.h"
LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    this->setAttribute(Qt::WA_QuitOnClose,true);
    isHotel=false;
    isReg=false;
    QSize size=QApplication::desktop()->size();
    this->setGeometry(size.width()/3,size.height()*3/8,size.width()/3,size.height()/4);
    this->setMaximumSize(size.width()/3,size.height()/4);
    this->setMinimumSize(size.width()/3,size.height()/4);
    this->setWindowTitle("欢迎使用酒店（管理/预定/？？）系统");
    pb1=new QPushButton(this);
    pb1->setWindowOpacity(0.3);
    pb1->setAttribute(Qt::WA_TranslucentBackground, false);
    pb1->setAutoFillBackground(true);
    pb1->setGeometry(0,0,size.width()/12,size.height()/8);
    QPixmap image1;
    image1.load(":/images/user.png");
    image1=image1.scaled(pb1->size());
    QIcon icon1(image1);
    pb1->setIcon(icon1);
    pb1->setIconSize(pb1->size());
    pb1->setVisible(true);
    pb1->setFlat(true);
    pb1->show();

    pb2=new QPushButton(this);
    pb2->setAttribute(Qt::WA_TranslucentBackground, false);
    pb2->setWindowOpacity(0.3);
    pb2->setAutoFillBackground(true);
    pb2->setGeometry(0,size.height()/8,size.width()/12,size.height()/8);
    QPixmap image2;
    image2.load(":/images/hotel.png");
    image2=image2.scaled(pb1->size());
    QIcon icon2(image2);
    pb2->setIcon(icon2);
    pb2->setIconSize(pb2->size());
    pb2->setVisible(true);
    pb2->setFlat(true);
    pb2->show();
    pb1->setFlat(false);

    pb3=new QPushButton(this);
    pb3->setGeometry(size.width()/4,0,size.width()/12,size.height()/8);
    pb3->show();
    pb3->setFlat(true);
    pb3->show();
    pb3->setText("Sign In");

    pb4=new QPushButton(this);
    pb4->setGeometry(size.width()/4,size.height()/8,size.width()/12,size.height()/8);
    pb4->show();
    pb4->setFlat(true);
    pb4->show();
    pb4->setText("Sign Up");

    pb3->setFlat(false);

    lab1=new QLabel(this);
    lab1->setGeometry(size.width()/12,20,size.width()/24,15);
    lab1->setText("用户名：");
    lab1->show();
    lab2=new QLabel(this);
    lab2->setGeometry(size.width()/12,50,size.width()/24,15);
    lab2->setText("电话号码：");
    lab2->show();
    lab3=new QLabel(this);
    lab3->setGeometry(size.width()/12,80,size.width()/24,15);
    lab3->setText("密码：");
    lab3->show();
    lab4=new QLabel(this);
    lab4->setGeometry(size.width()/12,110,size.width()/24,15);
    lab4->setText("经度：");
    lab4->show();
    lab5=new QLabel(this);
    lab5->setGeometry(size.width()/12+size.width()/12,110,size.width()/24,15);
    lab5->setText("纬度：");
    lab5->show();
    lab6=new QLabel(this);
    lab6->setGeometry(size.width()/12,140,size.width()/24,15);
    lab6->setText("详细介绍：");
    lab6->show();
    lab7=new QLabel(this);
    lab7->setGeometry(size.width()/12,size.height()*7/32,size.width()/24,size.height()/32);
    lab7->setText("城市：");
    lab7->show();
    connect(pb1,SIGNAL(clicked(bool)),this,SLOT(Usrclicked()));
    connect(pb2,SIGNAL(clicked(bool)),this,SLOT(Hotclicked()));
    connect(pb3,SIGNAL(clicked(bool)),this,SLOT(Logclicked()));
    connect(pb4,SIGNAL(clicked(bool)),this,SLOT(Regclicked()));
    user=new QLineEdit(this);
    user->setGeometry(size.width()/12+size.width()/36+size.width()/36,20,size.width()/9,15);
    tel=new QLineEdit(this);
    tel->setGeometry(size.width()/12+size.width()/36+size.width()/36,50,size.width()/9,15);
    pass=new QLineEdit(this);
    pass->setGeometry(size.width()/12+size.width()/36+size.width()/36,80,size.width()/9,15);
    pass->setEchoMode(QLineEdit::Password);
    x=new QLineEdit(this);
    y=new QLineEdit(this);
    city=new QComboBox(this);
    city->setGeometry(size.width()/12+size.width()/24,size.height()*7/32,size.width()/24,size.height()/32);
    city->addItems(*(Client::city_list));
    x->setGeometry(size.width()/8,110,size.width()/24,15);
    y->setGeometry(size.width()*5/24,110,size.width()/24,15);
    x->setValidator(new QDoubleValidator(0,180,5));
    y->setValidator(new QDoubleValidator(0,180,5));
    user->show();
    tel->show();
    pass->show();
    tel->hide();
    lab2->hide();
    lab4->hide();
    lab5->hide();
    lab6->hide();
    lab7->hide();
    city->hide();
    x->hide();
    y->hide();

    desc=new QLineEdit(this);
    desc->setGeometry(size.width()/12+size.width()/36+size.width()/36,140,size.width()/9,15);
    desc->hide();

    pb_ok=new QPushButton(this);
    pb_ok->setGeometry(size.width()/6,size.height()*7/32,size.width()/15,size.height()/32);
    pb_ok->setText("Confirm");
    pb_ok->setShortcut(Qt::Key_Return);
    connect(pb_ok,SIGNAL(clicked(bool)),this,SLOT(submit()));
    this->show();

}
void LoginDialog::Usrclicked(){
    isHotel=false;
    pb1->setFlat(false);
    pb2->setFlat(true);
    lab4->hide();
    lab5->hide();
    lab6->hide();
    lab7->hide();
    city->hide();
    x->hide();
    y->hide();
    desc->hide();
}
void LoginDialog::Hotclicked(){
    isHotel=true;
    pb1->setFlat(true);
    pb2->setFlat(false);
    if(isReg){
        lab4->show();
        lab5->show();
        lab6->show();
        lab7->show();
        city->show();
        x->show();
        y->show();
        desc->show();
    }else{
        lab4->hide();
        lab5->hide();
        lab6->hide();
        lab7->hide();
        city->hide();
        x->hide();
        y->hide();
        desc->hide();
    }
}
void LoginDialog::Logclicked(){
    isReg=false;
    pb3->setFlat(false);
    pb4->setFlat(true);
    tel->hide();
    lab2->hide();
    lab4->hide();
    lab5->hide();
    lab6->hide();
    lab7->hide();
    city->hide();
    x->hide();
    y->hide();
    desc->hide();
}
void LoginDialog::Regclicked(){
    isReg=true;
    pb3->setFlat(true);
    pb4->setFlat(false);
    tel->show();
    lab2->show();
    if(isHotel){
        lab4->show();
        lab5->show();
        lab6->show();
        lab7->show();
        city->show();
        x->show();
        y->show();
        desc->show();
    }else{
        lab4->hide();
        lab5->hide();
        lab6->hide();
        lab7->hide();
        city->hide();
        x->hide();
        y->hide();
        desc->hide();
    }
}
void LoginDialog::submit(){
    if(isHotel&&isReg){
        if(user->text().isEmpty()){
            MessageBoxW(0,L"请输入用户名",L"错误",MB_OK);
            return;
        }
        if(pass->text().isEmpty()){
            MessageBoxW(0,L"请输入密码",L"错误",MB_OK);
            return;
        }
        if(tel->text().isEmpty()){
            MessageBoxW(0,L"请输入电话号码",L"错误",MB_OK);
            return;
        }
        for(int i=0;i<tel->text().length();i++){
            if((tel->text().at(i).toLatin1()<'0')||(tel->text().at(i).toLatin1()>'9')){
                MessageBoxW(0,L"请输入纯数字电话号码",L"错误",MB_OK);
                return;
            }
        }
        bool ok;
        x->text().toDouble(&ok);
        if(!ok){
            MessageBoxW(0,L"请输入经度坐标",L"错误",MB_OK);
            return;
        }
        y->text().toDouble(&ok);
        if(!ok){
            MessageBoxW(0,L"请输入纬度坐标",L"错误",MB_OK);
            return;
        }
        if(desc->text().isEmpty()){
            MessageBoxW(0,L"请输入酒店介绍",L"错误",MB_OK);
            return;
        }

        QJsonObject obj;
        obj.insert("action","reg");
        obj.insert("type",1);
        QByteArray passRaw=pass->text().toLatin1();
        QByteArray passCip=QCryptographicHash::hash(passRaw,QCryptographicHash::Sha3_512);
        _512 key;
        key.FromRaw(passCip);
        qDebug()<<(QByteArray)key;
        obj.insert("pass",(QString)((QByteArray)key));
        obj.insert("username",user->text());
        obj.insert("phone",tel->text());
        obj.insert("x",x->text().toDouble());
        obj.insert("y",y->text().toDouble());
        obj.insert("desc",desc->text());
        obj.insert("city",city->currentIndex());
        Client::Sendmsg(&obj);
    }else
    if((!isHotel)&&isReg){
        if(user->text().isEmpty()){
            MessageBoxW(0,L"请输入用户名",L"错误",MB_OK);
            return;
        }
        if(pass->text().isEmpty()){
            MessageBoxW(0,L"请输入密码",L"错误",MB_OK);
            return;
        }
        if(tel->text().length()!=11){
            MessageBoxW(0,L"请输入手机号码",L"错误",MB_OK);
            return;
        }
        for(int i=0;i<11;i++){
            if((tel->text().at(i).toLatin1()<'0')||(tel->text().at(i).toLatin1()>'9')){
                MessageBoxW(0,L"请输入纯数字手机号码",L"错误",MB_OK);
                return;
            }
        }
        QJsonObject obj;
        obj.empty();
        obj.insert("action","reg");
        obj.insert("type",0);
        QByteArray passRaw=pass->text().toLatin1();
        QByteArray passCip=QCryptographicHash::hash(passRaw,QCryptographicHash::Sha3_512);
        _512 key;
        key.FromRaw(passCip);
        obj.insert("pass",(QString)((QByteArray)key));
        obj.insert("username",user->text());
        obj.insert("phone",tel->text());
        int rtn=Client::Sendmsg(&obj);
        if(rtn){
            if(rtn==-2)MessageBoxW(0,L"错误",L"用户名已存在",MB_OK);
            else{
                wchar_t errn[1024];
                QString("发生未知错误\n错误代码："+QString::number(rtn)+"\n错误信息："+Client::getLastError()).toWCharArray(errn);
                MessageBoxW(0,errn,L"错误",MB_OK);
            }
        }
        captcha=new QInputDialog(this);
        captcha->setInputMode(QInputDialog::TextInput);
        captcha->setLabelText("请输入您刚刚收到的验证码。");
        connect(captcha,SIGNAL(textValueChanged(QString)),this,SLOT(CheckCaptc(QString)));
        captcha->exec();
        int capt=captcha->textValue().toInt();
        qDebug()<<capt;
        obj.empty();
        obj.insert("action","confirm");
        obj.insert("vcode",capt);
        rtn=Client::Sendmsg(&obj);
        qDebug()<<Client::getReply();
    }else
    if(isHotel&&(!isReg)){
        if(user->text().isEmpty()){
            MessageBoxW(0,L"请输入用户名",L"错误",MB_OK);
            return;
        }
        if(pass->text().isEmpty()){
            MessageBoxW(0,L"请输入密码",L"错误",MB_OK);
            return;
        }
        QJsonObject obj;
        obj.insert("action","user");
        obj.insert("type",1);
        obj.insert("username",user->text());
        int rtn=Client::Sendmsg(&obj);
        if(rtn==0){
            QByteArray passRaw=pass->text().toLatin1();
            QByteArray passCip=QCryptographicHash::hash(passRaw,QCryptographicHash::Sha3_512);
            _512 key;
            key.FromRaw(passCip);
            QByteArray ttt=Client::getReply().toLatin1();
            _512 salt;
            salt=ttt;
            salt=salt^key;
            salt=salt.hash();
            obj.empty();
            obj.insert("action","pass");
            obj.insert("pass",QString::fromLatin1((QByteArray)salt));
            rtn=Client::Sendmsg(&obj);
            qDebug()<<rtn;

            if(rtn==0){this->hide(); Hotel2 form(nullptr);form.exec();}//this->accept();
            else MessageBoxW(0,L"登录失败~",L"登录",MB_OK);
        }
    }else
    if((!isHotel)&&(!isReg)){
        if(user->text().isEmpty()){
            MessageBoxW(0,L"请输入用户名",L"错误",MB_OK);
            return;
        }
        if(pass->text().isEmpty()){
            MessageBoxW(0,L"请输入密码",L"错误",MB_OK);
            return;
        }
        QJsonObject obj;
        obj.insert("action","user");
        obj.insert("type",0);
        obj.insert("username",user->text());
        int rtn=Client::Sendmsg(&obj);
        if(rtn==0){
            QByteArray passRaw=pass->text().toLatin1();
            QByteArray passCip=QCryptographicHash::hash(passRaw,QCryptographicHash::Sha3_512);
            _512 key;
            key.FromRaw(passCip);
            QByteArray ttt=Client::getReply().toLatin1();
            _512 salt;
            salt=ttt;
            salt=salt^key;
            salt=salt.hash();
            obj.empty();
            obj.insert("action","pass");
            obj.insert("pass",QString::fromLatin1((QByteArray)salt));
            rtn=Client::Sendmsg(&obj);
            qDebug()<<rtn;
            if(rtn==0)this->accept();else MessageBoxW(0,L"登录失败~",L"登录",MB_OK);
        }
    }
}
void LoginDialog::CheckCaptc(const QString& text){
    if(text.length()>4){
        captcha->setTextValue(tmp);
        return;
    }
    for(int i=0;i<text.length();i++)
        if((text.at(i).toLatin1()<'0')||(text.at(i).toLatin1()>'9')){
            captcha->setTextValue(tmp);
            return;
        }
    tmp=text;
    return;
}
