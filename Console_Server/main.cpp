#define QT_NO_DEBUG

#include <QCoreApplication>
#include "serverlet.h"
#include <QProcess>
#include<QUrl>
#include<QByteArray>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkReply>
#include<QtNetwork/QNetworkAccessManager>

#include<QEventLoop>
#include<io.h>
#include"windows.h"
#include<iostream>



int main(int argc, char *argv[])
{

    std::set_terminate([](){
        std::cout << "Unhandled exception"<<std::endl;
        std::abort();
    });


    if(argc==1){
        QCoreApplication a(argc, argv);
        Serverlet s;
        int tmp=a.exec();
        return tmp;
    }else if(argc==2){
        QCoreApplication a(argc, argv);
        fclose(stderr);
        int mon=(QString::fromLatin1(argv[1])).toInt();
        QNetworkAccessManager* pManager = new QNetworkAccessManager;
        QEventLoop *e=new QEventLoop;
        QNetworkReply *r;
        QByteArray iid;
        QNetworkRequest request;
        QSslConfiguration conf = request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        conf.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(conf);
        request.setUrl(QUrl("https://usereg.tsinghua.edu.cn/do.php"));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36");

        QByteArray bytePost="action=login&user_login_name=wangzw17&user_password=c3a5c91b9d192bb1ad44bab772ad73cc";
        //请使用自己的校园网账号与密码（hash）进行替换。

        r = pManager->post(request,bytePost);
        QCoreApplication::connect(r , SIGNAL(finished()) , e, SLOT(quit()));
        e->exec();
        request.setUrl(QUrl("https://usereg.tsinghua.edu.cn/buyapi/tqinghua.php?money="+QString::number(mon)));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36");

        r= pManager->get(request);
        QCoreApplication::connect(r , SIGNAL(finished()) , e, SLOT(quit()));
        e->exec();
        for(int i=0;i<24;i++)r->readLine();
        QByteArray par1=r->readLine().left(63).right(14);
        for(int i=0;i<3;i++)r->readLine();
        QByteArray par2=r->readLine().left(77).right(32);
        for(int i=0;i<6;i++)r->readLine();
        QByteArray par3=r->readLine().left(70).right(19);
        request.setUrl(QUrl("http://zhifu.tsinghua.edu.cn/sfpt/requestPayAction!payment.action"));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36");
        bytePost="t_partner=2112&t_return_url=http%3A%2F%2Fusereg.tsinghua.edu.cn%2Fbuyapi%2Freturn_url.php&t_version=1.0&t_datetime="+par1+"&t_extra_common_param=abcde&t_sign="+par2+"&t_business=wangfei&t_subitem=0&t_name=%D7%D4%D6%FA%CD%F8%B7%D1%B3%E4%D6%B5&t_summary=test&t_username=wangzw17&t_out_trade_no="+par3+"&t_total_fee="+QByteArray::number(mon)+".00&t_user_id=2017011520&t_user_id_type=0";
        r = pManager->post(request,bytePost);
        QCoreApplication::connect(r , SIGNAL(finished()) , e, SLOT(quit()));
        e->exec();
        for(int i=0;i<98;i++)r->readLine();
        iid=r->readLine().left(47).right(7);
        QByteArray xxx=r->readLine().left(111).right(60);
        request.setUrl(QUrl("http://zhifu.tsinghua.edu.cn/sfpt/checkPayAction!check.action"));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36");

        bytePost="xxx="+xxx;
        r = pManager->post(request,bytePost);
        QCoreApplication::connect(r , SIGNAL(finished()) , e, SLOT(quit()));
        e->exec();
        request.setUrl(QUrl("http://zhifu.tsinghua.edu.cn/sfpt/sendToWxpayAction!send.action"));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.62 Safari/537.36");

        bytePost="id="+iid+"&xxx="+xxx;
        r = pManager->post(request,bytePost);
        QCoreApplication::connect(r , SIGNAL(finished()) , e, SLOT(quit()));
        e->exec();
        for(int i=0;i<56;i++)r->readLine();
        QByteArray tmpb64;
        tmpb64=r->readLine();
        tmpb64=tmpb64.right(tmpb64.length()-tmpb64.indexOf("base")-7);
        tmpb64=tmpb64.left(tmpb64.length()-1);
        while(tmpb64.indexOf(">")==-1){
            tmpb64=tmpb64+r->readLine();
            tmpb64=tmpb64.left(tmpb64.length()-1);
        }
        tmpb64=tmpb64.left(tmpb64.indexOf("\""));
        QJsonObject obj;
        obj.insert("iid",QString::fromLatin1(iid));
        obj.insert("data",QString::fromLatin1(tmpb64));
        QJsonDocument doc(obj);
        std::cout<<QString(doc.toJson()).toStdString();
        delete r;
        delete e;
        delete pManager;
    }else{
        QCoreApplication a(argc, argv);
        fclose(stderr);
        QString iid=QString::fromLatin1(argv[2]);
        QNetworkAccessManager* pManager = new QNetworkAccessManager;
        QEventLoop *e=new QEventLoop;
        QNetworkReply *r;
        QNetworkRequest request;
        QSslConfiguration conf = request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        conf.setProtocol(QSsl::AnyProtocol);
        request.setSslConfiguration(conf);
        request.setUrl(QUrl("http://zhifu.tsinghua.edu.cn/sfpt/queryPayAction!query.action?t_id="+iid+"&t_use_type=2&t_version=1.2"));
        r = pManager->get(request);
        QCoreApplication::connect(r,SIGNAL(finished()),e,SLOT(quit()));
        if(e->isRunning()){
            std::cout<<"Wait";
            return 0;
        }
        e->exec();
        QByteArray tmp=r->readAll();
        qDebug()<<tmp;
        QString str=QString::fromLocal8Bit(tmp);
        tmp=str.toUtf8();
        qDebug()<<tmp;
        QJsonParseError jsonError;
        QJsonDocument json_doc=QJsonDocument::fromJson(tmp, &jsonError);
        qDebug()<<json_doc.toJson();

        if(json_doc.isObject())qDebug()<<"Yes";else {
            std::cout<<"Error";
            return 0;
        }
        QJsonObject json = json_doc.object();

        if(!json.contains("err")){
            std::cout<<"Error";
            return 0;
        }
        int err=json.take("err").toInt(-2);
        if(err<0){
            std::cout<<"Error";
            return 0;
        }
        if(err>0){
            std::cout<<"Wait";
            return 0;
        };
        if(err==0){
            if((!json.contains("url"))||(json.take("url").isNull())){
                std::cout<<"Error";
                return 0;
            }
            else{
                std::cout<<"Good";
                return 0;
            }
        }
        return -1;
        delete r;
        delete e;
        delete pManager;

    }
}
