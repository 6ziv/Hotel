#ifndef USERTRADES_H
#define USERTRADES_H
#include<QListWidget>
#include<QVector>
#include"mainwindow.h"
#include <QWidget>
#include <QDialog>
class UserTrades;
class TradeInfo_user{
public:
    int id;
    QString displayname;
    int start;
    int finish;
    int price;
    double star;
    UserTrades *p;
};
class TradeItem_user:public QWidget
{
    Q_OBJECT
private:
    Stars *stars;
    QPushButton *pb;
    TradeInfo_user info;
public:
    TradeItem_user(QWidget* parent,TradeInfo_user in,int type);
    ~TradeItem_user();
public slots:
    void star();
    void rf();
  //  void mouseReleaseEvent(QMouseEvent *e);
};
class User_Trade_List:public QWidget
{
    Q_OBJECT
public:
    int type;
    QVector<TradeInfo_user>trades;
    User_Trade_List(QWidget *parent,int p_type);
    ~User_Trade_List();

    QListWidget *lw;
};
class UserTrades:public QDialog
{
    Q_OBJECT
private:
    User_Trade_List *page1;
    User_Trade_List *page2;
public:
    UserTrades(QWidget *parent);
    ~UserTrades();
    void refresh();
};

#endif // USERTRADES_H
