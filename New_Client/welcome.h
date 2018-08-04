#ifndef WELCOME_H
#define WELCOME_H
#include<QTimer>
#include<QApplication>
#include<QDialog>
#include<QDesktopWidget>
#include"logindialog.h"
class Welcome
        :public QDialog
{
    Q_OBJECT
private:
    LoginDialog *w;
    QTimer *timer;
public:
    Welcome(QWidget *parent=0);
    ~Welcome();
public slots:
    void next();
};
#endif // WELCOME_H
