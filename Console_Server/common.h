#ifndef COMMON_H
#define COMMON_H
#include<QList>
#include<QtSql/QSql>
#include<QtSql/QSqlDatabase>
#include<QtSql/QSqlQuery>
#include<QObject>
#include<QTcpServer>
#include<QTcpSocket>
#include<QAbstractSocket>
#include<iostream>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QMutex>
#include<QMap>
#include<QMapData>
#include<QChar>
#include<QRandomGenerator>
#include<QCryptographicHash>
#include"time.h"
#include"mutex"
//错误常数
const int ERROR_NO_ERROR=0;
const int ERROR_NO_SUCH_USER=-1;
const int ERROR_USER_EXISTS=-2;
const int ERROR_ACCESS_DENIED=-3;
const int ERROR_NO_SUCH_COMMAND=-4;
const int ERROR_LACK_PARAMETERS=-5;
const int ERROR_WAITING=-6;
const int ERROR_NO_EMPTY_ROOM=-7;
const int ERROR_INVALID_PARAMETER=-8;
const int ERROR_STATE_NOT_READY=-9;
const int ERROR_FILE_NOT_FOUND=-10;
const int ERROR_WRONG_PASSWORD=-11;
const int ERROR_WRONG_CAPTCHA=-12;
const int ERROR_UNKNOWN_ERROR=-99;

//交易状态
enum Trade_State{
    TRADE_NOT_PAYED=10,
    TRADE_PAYED=11,
    TRADE_REFUNDING=12,
    TRADE_REFUNDED=13,
    TRADE_ABONDONED=14,
    TRADE_DONE=15
};

//用户状态
enum User_State{
    USER_NOT_LOGGED_IN=20,
    USER_CUSTOMER=21,
    USER_HOTEL=22,
    USER_ADMIN=23,
    USER_HANGING=24
};
#endif // COMMON_H
