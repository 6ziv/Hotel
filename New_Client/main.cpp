#include "mainwindow.h"
#include "welcome.h"
#include "book.h"
#include"usertrades.h"
#include <QApplication>
#include<QNetworkConfiguration>
#include<QNetworkAccessManager>
#include<QNetworkReply>

int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    MainWindow w;
    w.show();

    return a.exec();
}

