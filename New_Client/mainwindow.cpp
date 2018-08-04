#include "mainwindow.h"
#include "usertrades.h"
#include "welcome.h"
#include"hotel_detail.h"
#include<QJsonArray>
#include"password.h"
#include<algorithm>
#include<QJsonDocument>
#include<QCryptographicHash>
#include<QJsonObject>
#include<QByteArray>
#include<QBitmap>
#include<QRadioButton>
#include"chpwd.h"
#include<QListWidget>
#include<QComboBox>
#include"welcome.h"
#include"windows.h"
QWidget* Position::window;
QAxWidget* Position::web;
QString Position::path;
Position::Position(QWidget *parent)
    :QPushButton(parent)
{
    setGeometry(0,0,200,200);
    show();
    init();
}
void Position::enterEvent(QEvent *){
    window->show();
    window->resize(802,702);
    web->resize(800,700);
    QString tmp=path;
    qDebug()<<tmp;
    tmp.append("?x=").append(QString::number(x,10,30)).append("&y=").append(QString::number(y,10,30)).append("&t=").append(name);
    web->dynamicCall("Navigate(const QString&)",tmp);
    qDebug()<<tmp;
    web->show();
}
void Position::init(){
    window=new QDialog();
    qDebug()<<"enter!";
    window->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    window->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);
    wchar_t tmp[1024];
    GetModuleFileNameW(0,tmp,1024);
    path=QString::fromWCharArray(tmp,1024);
    int i=path.length()-1;
    while(path.at(i)!=QChar('\\'))i--;
    path=path.left(i+1);
    path.append("1.html");
    path="file://"+path.replace("\\","/");
    qDebug()<<path;

    web=new QAxWidget(window);
    web->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
    web->setObjectName(QString::fromUtf8("webWidget"));//设置控件的名称
    web->setFocusPolicy(Qt::StrongFocus);//设置控件接收键盘焦点的方式：鼠标单击、Tab键
    web->setProperty("DisplayAlerts",false); //不显示任何警告信息。
    web->setProperty("DisplayScrollBars",false); // 显示滚动条
    window->setGeometry(0,0,800,700);
    SetWindowLong((HWND)(window->winId()), GWL_EXSTYLE, GetWindowLong((HWND)(window->winId()), GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    web->setGeometry(0,0,800,700);
}
void Position::leaveEvent(QEvent *){
    qDebug()<<"leave!";
    window->hide();
}
void Position::setPosition(qreal p_x, qreal p_y,QString p_name){
    x=p_x;y=p_y;name=p_name;
    this->setText("地址");
}
Stars::Stars(QWidget *parent)
    :QProgressBar(parent)
{
    QBitmap mask;
    QPixmap pixmap;
    pixmap.load(":/images/Stars.png");
    pixmap=pixmap.scaled(this->size());
    mask=pixmap.mask();
    setMask(mask);
    setStyleSheet("QProgressBar::chunk {background: rgb(255,215,0, 60%);}");
    setTextVisible(false);
    show();
}
void Stars::resizeEvent(QResizeEvent *event){
    qDebug()<<"resize";
    QProgressBar::resizeEvent(event);
    QBitmap mask;
    QPixmap pixmap;
    pixmap.load(":/images/Stars.png");
    pixmap=pixmap.scaled(this->size());
    mask=pixmap.mask();
    setMask(mask);
    this->update();
}
void Stars::setstars(qreal stars){
    this->setValue((int)(stars*20));
    this->update();
}
item::item(QWidget *parent,HotelSimpInfo &info)
    :QWidget(parent)
{
    QSize size=QApplication::desktop()->size();
    this->resize(size.width()/2-20,size.height()/5);
    this->setMinimumSize(QSize(size.width()/2-20,size.height()/5));

    QLabel* name=new QLabel(this);
    name->setText(info.name);
    name->setGeometry(0,0,size.width()*3/3-20,20);
    QFont font=name->font();
    font.setPointSize(15);
    name->setFont(font);
    name->show();
    Stars* stars=new Stars(this);
    stars->setGeometry(0,20,size.width()/8,20);
    stars->resize(size.width()*3/32,20);
    stars->show();
    stars->setstars(info.stars);

    QLabel* desc=new QLabel(this);
    desc->setText(info.desc);
    desc->setGeometry(size.width()/8,0,size.width()/4-100,40);

    QLabel* tel=new QLabel(this);
    tel->setText("Tel:"+info.tel);
    tel->setGeometry(size.width()*13/32-100,0,size.width()*3/32,20);
    tel->show();

    QLabel* low=new QLabel(this);
    low->setText("￥"+QString::number(info.lowest)+"起");
    low->setGeometry(size.width()*13/32-100,20,size.width()/16,20);
    low->show();


    Position* pos=new Position(this);
    pos->setPosition(info.x,info.y,info.name);
    pos->setGeometry(size.width()*15/32-100,20,size.width()/32,20);
    pos->show();
    b.desc=info.desc;
    b.id=info.id;
    b.lowest=info.lowest;
    b.name=info.name;
    b.stars=info.stars;
    b.tel=info.tel;
    b.x=info.x;
    b.y=info.y;

    this->show();
    this->update();
}
void item::mouseReleaseEvent(QMouseEvent *event){
    QWidget::mouseReleaseEvent(event);
    Hotel_Detail d(this,b);
    d.exec();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Client::city_list=new QStringList;
    Client::city_list->push_back("City1");
    Client::city_list->push_back("City2");
    Client::city_list->push_back("City3");
    Client::city_list->push_back("City4");
    Client::city_list->push_back("City5");
    Client::city_list->push_back("City6");
    this->setAttribute(Qt::WA_QuitOnClose);
    Welcome *welcome=new Welcome(this);
    bool rtn=welcome->exec();
    delete welcome;
    LoginDialog *Login=new LoginDialog(this);
    rtn=Login->exec();
    qDebug()<<"done";
    delete Login;
    if(!rtn)delete this;

    QLabel *lab=new QLabel(this);
    lab->setGeometry(10,10,50,30);
    lab->setText("城市：");
    city=new QComboBox(this);
    city->setGeometry(60,10,60,30);
    city->addItems(*(Client::city_list));
    QPushButton *search=new QPushButton(this);
    search->setText("Search!");
    search->setGeometry(130,10,50,30);
    connect(search,SIGNAL(clicked(bool)),this,SLOT(do_search()));
    QLabel *lab2=new QLabel(this);
    lab2->setGeometry(200,10,30,30);
    lab2->setText("排序：");
    lab2->show();
    radio1=new QRadioButton(this);
    radio1->setText("By stars");

    radio2=new QRadioButton(this);
    radio2->setText("By price");
    radio1->setGeometry(240,10,80,30);
    radio1->setChecked(true);
    radio2->setGeometry(330,10,80,30);
    connect(radio1,SIGNAL(toggled(bool)),this,SLOT(orderby()));
    connect(radio2,SIGNAL(toggled(bool)),this,SLOT(orderby()));

    QSize size=QApplication::desktop()->size();
    QPushButton *chPwd=new QPushButton(this);
    chPwd->setGeometry(size.width()/2-70,10,60,30);
    chPwd->setText("更改密码");
    connect(chPwd,SIGNAL(clicked(bool)),this,SLOT(chpwd()));
    QPushButton *trades=new QPushButton(this);
    trades->setGeometry(size.width()/2-70-70,10,60,30);
    trades->setText("查看交易");
    connect(trades,SIGNAL(clicked(bool)),this,SLOT(trades()));


    this->setGeometry(size.width()/4,size.height()/4,size.width()/2,size.height()/2);
    this->setMaximumSize(size.width()/2,size.height()/2);
    this->setMinimumSize(size.width()/2,size.height()/2);
    hotels.clear();
    lw=new QListWidget(this);
    lw->setGeometry(10,50,size.width()/2-20,size.height()/2-55);



}

bool func1(HotelSimpInfo a,HotelSimpInfo b){
    return a.stars>b.stars;
}
bool func2(HotelSimpInfo a,HotelSimpInfo b){
    return a.lowest<b.lowest;
}
void MainWindow::do_search(){
    QJsonObject obj;
    obj.empty();
    obj.insert("action","all_hotel");
    obj.insert("city",city->currentIndex());
    Client::Sendmsg(&obj);
    QByteArray data=Client::getReply().toLatin1();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    if(doc.isArray()){
        hotels.clear();
        QJsonArray arr=doc.array();
        for(int i=0;i<arr.size();i++){
            HotelSimpInfo inf;
            QJsonObject obj=arr.at(i).toObject();
            inf.desc=obj.take("desc").toString();
            inf.id=obj.take("id").toInt();
            inf.name=obj.take("name").toString();
            inf.stars=obj.take("star").toDouble();;
            inf.tel=obj.take("tel").toString();
            inf.x=obj.take("x").toDouble();
            inf.y=obj.take("y").toDouble();
            inf.lowest=obj.take("lowest").toInt();;
            hotels.push_back(inf);
        }
        orderby();
    }
}
void MainWindow::orderby(){
    QSize size=QApplication::desktop()->size();
    lw->clear();
    if(radio1->isChecked()){
        std::sort(hotels.begin(),hotels.end(),func1);
    }else{
        std::sort(hotels.begin(),hotels.end(),func2);
    }
    for(QVector<HotelSimpInfo>::iterator iter=hotels.begin();iter!=hotels.end();iter++){
        item *itm=new item(this,*iter);
        QListWidgetItem *Item=new QListWidgetItem(lw);
        Item->setSizeHint(QSize(size.width()/2-45,50));
        lw->addItem(Item);
        lw->setItemWidget(Item,itm);
    }
}
MainWindow::~MainWindow()
{
    qDebug()<<"Bye~";
}
void Stars::mouseReleaseEvent(QMouseEvent *event){
    emit clicked();
}
void MainWindow::chpwd(){
    QString rtn;
    ChPwd *cpw=new ChPwd(this,&rtn);
    int rrr=cpw->exec();
    delete cpw;
    qDebug()<<rtn;
    cpw=0;
    if(rrr!=QDialog::Rejected)return;
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

void MainWindow::trades(){
    QString rtn;
    UserTrades *cpw=new UserTrades(0);
    cpw->exec();
    delete cpw;

}
