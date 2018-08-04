#include "client.h"
#include "windows.h"
QEventLoop* Client::e;
std::mutex Client::m_lock;
QString Client::error;
QString Client::reply;

QTcpSocket Client::socket;
QStringList* Client::city_list;
int Client::avai(){
   return (int)socket.bytesAvailable();
}
int Client::Sendmsg(QJsonObject *obj){
    if(socket.state()!=QTcpSocket::SocketState::ConnectedState){
        MessageBoxW(0,L"无法连接到服务器。",L"Error",MB_OK);
        exit(0);
    };

    m_lock.lock();
        QJsonDocument doc(*obj);
        QByteArray buffer=doc.toJson();
        socket.write(buffer);
        socket.flush();
        QObject::connect(&socket,SIGNAL(readyRead()),e,SLOT(quit()));
        e->exec();
        buffer.clear();
        buffer=socket.readAll();
        qDebug()<<buffer;
        doc=QJsonDocument::fromJson(buffer);
        if(!doc.isObject()){
            m_lock.unlock();
            error="Not understood.";
            return -99;
        }
        QJsonObject obj1=doc.object();
        int code=obj1.value("code").toInt();
        if(code<0)error=obj1.value("msg").toString();else reply=obj1.value("msg").toString();
        m_lock.unlock();
        return obj1.value("code").toInt();
}

bool Client::connect(){
    e=new QEventLoop;
    socket.connectToHost("localhost",518);
    return true;
}
QString Client::getLastError(){
    return error;
}
QString Client::getReply(){
    return reply;
}
