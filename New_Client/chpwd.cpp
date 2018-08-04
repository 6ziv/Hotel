#include "chpwd.h"
#include "ui_chpwd.h"
#include<QDebug>
ChPwd::ChPwd(QWidget *parent,QString *des) :
    QDialog(parent),
    ui(new Ui::ChPwd)
{
    str=des;
    ui->setupUi(this);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(do_change()));
}

ChPwd::~ChPwd()
{
    *str=ui->lineEdit->text();
    delete ui;
}
void ChPwd::do_change(){
    if(ui->lineEdit->text().isEmpty())return;
    if(ui->lineEdit->text().isNull())return;
    this->accept();
}
