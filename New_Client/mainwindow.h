#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QAxWidget>
#include<QListWidget>
#include<QComboBox>
#include<QRadioButton>
#include<QDebug>
#include <QMainWindow>
#include<QLabel>
#include<QString>
#include<QEventLoop>
#include<QVector>
#include<QMouseEvent>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QPushButton>
#include<QProgressBar>
class UserSimpInfo{
public:
    QString username;
    int id;
};
class RoomSimpInfo{
public:
    int id;
    QString type;
    int price;

};
class HotelSimpInfo
{
public:
    int id;
    int lowest;
    QString name;
    QString desc;
    QString tel;
    double x;
    double y;
    double stars;
};

class Position:public QPushButton{
    Q_OBJECT
private:
    QString desc;
    qreal x;
    qreal y;
    static QWidget *window;
    static QAxWidget *web;
    static QString path;
    QString name;
public:
    void setPosition(qreal x,qreal y,QString name);
    static void init();
    Position(QWidget *parent=nullptr);
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};
class Stars:public QProgressBar{
    Q_OBJECT
public:
    void setstars(qreal stars);
    Stars(QWidget *parent=nullptr);
signals:
    void clicked(void);
protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);
};

class item:public QWidget{
    Q_OBJECT
private:
    HotelSimpInfo b;
public:
    item(QWidget *parent,HotelSimpInfo &info);
    void mouseReleaseEvent(QMouseEvent *event);
};
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QRadioButton *radio1;
    QRadioButton *radio2;
    QListWidget *lw;
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QComboBox *city;
    QVector<HotelSimpInfo> hotels;
public slots:
    void do_search();
    void orderby();
    void chpwd();
    void trades();
};

#endif // MAINWINDOW_H
