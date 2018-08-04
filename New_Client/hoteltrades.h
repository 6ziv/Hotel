#ifndef HOTELTRADES_H
#define HOTELTRADES_H
#include<QListWidget>
#include<QVector>
#include"mainwindow.h"
#include <QWidget>
#include <QDialog>
class HotelTrades;
class TradeInfo_hotel{
public:
    int id;
    int start;
    int finish;
    int price;
    HotelTrades *p;
};
class TradeItem_hotel:public QWidget
{
    Q_OBJECT
private:
    TradeInfo_hotel info;
public:
    TradeItem_hotel(QWidget* parent,TradeInfo_hotel in,int type);
    ~TradeItem_hotel();
public slots:
    void cr();

};

class HotelTrades:public QDialog
{
    Q_OBJECT
private:
    QVector<TradeInfo_hotel>trades;
    QListWidget *lw;
public:
    HotelTrades(QWidget *parent);
    ~HotelTrades();
    void refresh();
};

#endif // HOTELTRADES_H
