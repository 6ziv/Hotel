#ifndef HOTEL_DETAIL_H
#define HOTEL_DETAIL_H
#include"mainwindow.h"
#include <QDialog>
#include<QWidget>
class RoomItem : public QWidget
{
    Q_OBJECT
private:
    int id;
private slots:
//    void book();
public:
//    RoomItem();
};
class item2:public QWidget{
    Q_OBJECT
private:
    RoomSimpInfo b;
public:
    item2(QWidget *parent,RoomSimpInfo &info);
    void mouseReleaseEvent(QMouseEvent *event);
};

class Hotel_Detail : public QDialog
{
    Q_OBJECT

public:
    explicit Hotel_Detail(QWidget *parent,HotelSimpInfo info);
    ~Hotel_Detail();
private:
    QVector<RoomSimpInfo> rooms;
};

#endif // HOTEL_DETAIL_H
