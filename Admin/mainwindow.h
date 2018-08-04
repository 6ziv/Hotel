#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QListWidget>
class MainWindow;
class item:public QWidget{
    Q_OBJECT
public:
    int id;
    QString name;
    QString tel;
    item(QWidget *parent,int p_id,QString p_name,QString p_tel);
    static MainWindow *p;
public slots:
    void validit();
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QListWidget *lw;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void refresh();
};

#endif // MAINWINDOW_H
