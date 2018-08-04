#ifndef HOTEL2_H
#define HOTEL2_H
#include"rooms.h"
#include <QDialog>
#include<QLabel>
#include<QPushButton>
#include<QLineEdit>
class Hotel2:public QDialog
{
    Q_OBJECT
public:
    Hotel2(QWidget *parent);
private:
    QLabel *Lname;
    QLabel *Ltel;
    QLabel *Ldesc;
    QPushButton *name;
    QPushButton *chdesc;
    QPushButton *desc;
    QPushButton *tel;
    QPushButton *chtel;
    QPushButton *chpwd;
    QPushButton *Orders;
    QPushButton *Rooms;
    QLineEdit *edittel;
    QLineEdit *editdesc;
    void refresh();
    QString strname;
    QString strtel;
    QString strdesc;
private slots:
    void ctel();
    void cdesc();
    void do_tel();
    void do_desc();
    void cp();
    void r();
    void t();
};

#endif // HOTEL2_H
