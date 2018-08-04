#ifndef BOOK_H
#define BOOK_H
#include<QDate>
#include<QDateEdit>
#include<QLabel>
#include<QPushButton>
#include<QWidget>
#include<QDialog>
#include"mainwindow.h"
#include<QLineEdit>
class Book:public QDialog
{
    Q_OBJECT
private:
    RoomSimpInfo info;
    QDateEdit *date1;
    QDateEdit *date2;
private slots:
    void change();
    void go();
public:
    Book(QWidget *parent,RoomSimpInfo in);
    QLineEdit *number;
    QLineEdit *total;

};

#endif // BOOK_H
