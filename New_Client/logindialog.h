#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include<QApplication>
#include<QDesktopWidget>
#include"client.h"
#include <QWidget>
#include <QPushButton>
#include<QComboBox>
#include<QLineEdit>
#include<QLabel>
#include<QDialog>
#include<QInputDialog>
class LoginDialog : public QDialog
{
    Q_OBJECT
private:
    bool isHotel;
    bool isReg;
    QPushButton *pb1;
    QPushButton *pb2;
    QPushButton *pb3;
    QPushButton *pb4;
    QLineEdit *user;
    QLineEdit *tel;
    QLineEdit *pass;
    QLineEdit *x;
    QLineEdit *y;
    QLineEdit *desc;
    QComboBox *city;
    QLabel *lab1;
    QLabel *lab2;
    QLabel *lab3;
    QLabel *lab4;
    QLabel *lab5;
    QLabel *lab6;
    QLabel *lab7;
    QPushButton *pb_ok;
    QString tmp;
    QInputDialog *captcha;
public:
    explicit LoginDialog(QWidget *parent = nullptr);
public slots:
    void Usrclicked();
    void Hotclicked();
    void Logclicked();
    void Regclicked();
    void submit();
    void CheckCaptc(const QString& text);
};

#endif // LOGINDIALOG_H
