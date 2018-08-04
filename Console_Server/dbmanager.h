#ifndef DBMANAGER_H
#define DBMANAGER_H
#include<common.h>
#include<QMap>
#include<QTimer>
#include<QVector>
#include<QQueue>
class RoomInfo
{
public:
    int id;
    QString desc;
    QString type;
    unsigned int parent;
    unsigned int price;
    unsigned int number;
};
class HotelInfo
{
public:
    QVector<int> rooms;
    int id;
    QString name;
    QString desc;
    QString tel;
    double x;
    double y;
    unsigned int city;

    unsigned int lowest;
    unsigned int starred;
    unsigned long long total;
};

typedef QVector<int> Hotels_In_A_City;

class DBManager
{
private:
    static QSqlDatabase db;
    static QMutex m_db;
    static QMap<int,HotelInfo> hotels;
    static QMap<int,RoomInfo> rooms;
    static QVector<Hotels_In_A_City> C_H;
    //酒店信息与房间信息入内存
    //订单信息与用户信息不入内存：订单量级远超酒店/房间，无法放入内存；用户信息只有用户自己使用，利用率低。

public:
    static void init();

    static int ReadKey(QString username,QByteArray& password);
    static int ReadHotKey(QString username,QByteArray& password);

    static int IsUser(QString username, QString phone);
    static int AddUser(QString username,  QString phone,QString password);

    static int IsHotel(QString name);
    static int AddHotel(QString name,QString tel, double x,double y,int city ,QString password,QString description);

    static int ChKey(int id,QByteArray password);
    static int ChHotKey(int id,QByteArray password);

    static int ChHotDesc(int id,QString desc);
    static int ChHotTel(int id,QString tel);

    static int NewRoom(int parent,QString type,QString name,int price,int number);
    static int ChangeRoom(int id,int parent,QString type,QString name,int price,int number);

    static int ToValid(QJsonArray &array);
    static int Valid(int id);
    static int hasLeft(int id,int date);
    static int GoodMorning(int day);
    static int Book(int user,int room,int start,int finish,int *tradeid);
    static int Refund(int trade,int user);

    static int Checkin(int trade,int hotel);
    static int Timeout(int tradeid);
    static int Verify_Refund(int trade,int hotel);

    static int Read_Hotel(int hotel,QJsonObject &obj);
    static int Read_Hotels(int city,QJsonArray &array);
    static int Read_Rooms(int hotel,QJsonArray &array);
//    static int Read_AllRooms(int city,QString type,int maxPrice,QJsonArray &array);
    static int Read_TradesByUser(int id,QJsonArray &array);
    static int Read_TradesByHotel(int id,QJsonArray &array);
    static int SendCode(int code,QString dest);
    static int Star(int trade,int star);
  //  static int Room_Detail(int id,QJsonObject &obj);
    static int Pay(int tradeid);

    static QTcpSocket socket;
    static QMutex m_cell;
};


#endif // DBMANAGER_H
