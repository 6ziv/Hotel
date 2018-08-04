#ifndef CLIENT_H
#define CLIENT_H
#include<QEventLoop>
#include<QJsonDocument>
#include<QJsonObject>
#include<QTcpSocket>
#include<QString>
#include<mutex>
class Client
{
private:
    static QTcpSocket socket;
    static QEventLoop* e;
    static std::mutex m_lock;
    static QString error;
    static QString reply;
public:
    static QStringList* city_list;
    static int Sendmsg(QJsonObject *doc);
    static QString getLastError();
    static QString getReply();
    static bool connect();
    static int avai();
public slots:
    static void Disconnect();
};

#endif // CLIENT_H
