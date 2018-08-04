#ifndef CHPWD_H
#define CHPWD_H

#include <QDialog>

namespace Ui {
class ChPwd;
}

class ChPwd : public QDialog
{
    Q_OBJECT

public:
    explicit ChPwd(QWidget *parent = 0,QString *des=0);
    ~ChPwd();
    QString*str;
private slots:
    void do_change();
private:
    Ui::ChPwd *ui;
};

#endif // CHPWD_H
