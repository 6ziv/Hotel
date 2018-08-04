#ifndef ROOMSASD_H
#define ROOMSASD_H
#include<QDialog>
#include<QVector>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QListWidget>
#include<QListWidgetItem>
#include<QString>
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
#include <QWidget>
class RoomInf{
public:
    int id;
    QString type;
    QString desc;
    int price;
    int number;
};
class RoomsP;
class RoomItem2:public QWidget{
    Q_OBJECT
private:
    RoomsP *trueparent;
    RoomInf data;
    QLabel *type;
    QLabel *desc;
    QLabel *price;
    QLabel *number;
    QLineEdit *edittype;
    QLineEdit *editdesc;
    QLineEdit *editprice;
    QLineEdit *editnumber;
    QPushButton *confirm;
    QPushButton *undo;
    QPushButton *editit;
public:
    RoomItem2(QWidget *parent,RoomInf info,RoomsP *tp);
public slots:
    void edit();
    void save();
    void cancel();

};
class RoomsP:public QDialog
{
    Q_OBJECT
public:
    RoomItem2 *editing;
    RoomsP(QWidget *parent);
    QListWidget *list;
    void refresh();
};

#endif // ROOMS_H
