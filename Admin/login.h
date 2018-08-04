#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include<QDialog>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
class Login:public QDialog
{
    Q_OBJECT
private:
    QLineEdit *uname;
    QLineEdit *pass;
public:
    Login(QWidget *parent);
private slots:
    void dologin();
};

#endif // LOGIN_H
