#ifndef SESSION_H
#define SESSION_H
#include "common.h"
#include "dbmanager.h"
#include "serverlet.h"
#include "password.h"

class Session;
class User{
    friend class Customer;
    friend class Hotel;
private:
    Session *m_parent;
public:
    User(Session *parent);
    virtual ~User();
    unsigned int id;
    virtual bool logout();
    virtual bool mydetail(QJsonObject obj);
    virtual bool chkey(QJsonObject obj);
    virtual bool all_hotel(QJsonObject obj);
    virtual bool all_room(QJsonObject obj);
    virtual bool book(QJsonObject );
    virtual bool refund(QJsonObject );
    virtual bool alltrades(QJsonObject );
    virtual bool chroom(QJsonObject );
    virtual bool newroom(QJsonObject );
    virtual bool changedescription(QJsonObject );
    virtual bool changetel(QJsonObject obj);
    virtual bool confirm_refund(QJsonObject obj);
    virtual bool check_pay(QJsonObject obj);
    virtual bool confirm_hotel(QJsonObject obj);
    virtual bool review(QJsonObject obj);
};
class Customer:public User{
private:
    QString *iid;
    int *tradeid;
public:
    Customer(Session *parent):User(parent){
        iid=new QString;
        tradeid=new int;
    }
    virtual ~Customer();
    virtual bool chkey(QJsonObject obj);
    virtual bool book(QJsonObject obj);
    virtual bool refund(QJsonObject obj);
    virtual bool review(QJsonObject obj);
    virtual bool check_pay(QJsonObject obj);
    virtual bool alltrades(QJsonObject obj);
};
class Hotel:public User{
public:
    virtual ~Hotel();
    Hotel(Session *parent):User(parent){}
    virtual bool chkey(QJsonObject obj);
    virtual bool mydetail(QJsonObject obj);
    virtual bool chroom(QJsonObject obj);
    virtual bool newroom(QJsonObject obj);
    virtual bool changedescription(QJsonObject obj);
    virtual bool changetel(QJsonObject obj);
    virtual bool confirm_refund(QJsonObject obj);
    virtual bool all_room(QJsonObject obj);
    virtual bool alltrades(QJsonObject obj);
};
class Session:public QObject{
    Q_OBJECT
    friend class User;
    friend class Customer;
    friend class Hotel;
private :
    std::mutex m_lock;
    User *usr;
    QTcpSocket *socket;
    PASSWORD *password;
    unsigned int v_code;
    QJsonObject history;
    QJsonObject hist;
    void reg(QJsonObject obj);
    void confirm(QJsonObject obj);
    void user(QJsonObject obj);
    void pass(QJsonObject obj);
    void admlogin(QJsonObject obj);
    void admgetall(QJsonObject obj);
    void admvalid(QJsonObject obj);
    void SendMsg(int code,const QString& msg,bool lock=true);
  public:
    Session(QTcpSocket *sock);
    ~Session();
    static Serverlet* serverlet;
private slots:
    void do_switch();
    void DisConnected();
};
#endif
