#define QT_NO_DEBUG

#include"session.h"
#include<QFile>
#include<QProcess>
#include<QCoreApplication>
#include<QIODevice>
#include<QByteArray>
#include<QEventLoop>
#include<io.h>
#include"windows.h"
#include<QCoreApplication>
Serverlet* Session::serverlet;
Session::Session(QTcpSocket *sock){
    try{
        std::cout<<"New client connected"<<std::endl;
        usr=0;
        password=0;
        socket=sock;
        v_code=99999;
        QObject::connect(socket,SIGNAL(readyRead()),(QObject*)this,SLOT(do_switch()));
        QObject::connect(socket,SIGNAL(disconnected()),(QObject*)this,SLOT(DisConnected()));
    }catch(...){
        delete this;
    }
}
Customer::~Customer(){
    delete iid;
    delete tradeid;
}
Hotel::~Hotel(){
}
Session::~Session(){
    try{
        if(usr!=0)delete usr;
    }catch(...){}
    try{
        if(password!=0)delete password;
    }catch(...){}
    try{
        if(socket->isValid()){
            if(socket->isOpen())socket->close();
        }
    }catch(...){}
    try{
        socket->deleteLater();
    }catch(...){}
    std::cout<<"Disconnected!"<<std::endl;
}
void Session::SendMsg(int code,const QString& msg,bool lock){
    QJsonObject obj;
    obj.empty();
    obj.insert("code",code);
    obj.insert("msg",msg);
    QByteArray tmp=QJsonDocument(obj).toJson();
    try{
        if(lock)m_lock.lock();
        this->socket->write(tmp);
        if(lock)m_lock.unlock();
    }catch(...){};
    return;
}

void Session::DisConnected(){
    delete this;
}
void Session::reg(QJsonObject obj){
    if(!obj.contains("type")){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack user type");
        return;
    }
    int mode=obj.value("type").toInt(-1);
    if((mode!=0)&&(mode!=1)){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack user type");
        return;
    }
    if(!obj.contains("username")){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack user name");
        return;
    }
    if(!obj.contains("pass")){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack password");
        return;
    }
    QString tmp=obj.value("pass").toString();
    if(tmp.length()<128){
        SendMsg(ERROR_INVALID_PARAMETER,"Password error!");
        return;
    }
    for(int i=0;i<128;i++){
        char ch=tmp.toStdWString().c_str()[i];
        if(((ch<'0')||(ch>'9'))&&((ch<'A')||(ch>'F'))){
            SendMsg(ERROR_INVALID_PARAMETER,"Password error");
            return;
        }
    }

    if(!obj.contains("phone")){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack phone number");
        return;
    }
    if(mode==0){
        tmp=obj.value("phone").toString();
        if(tmp.length()!=11){
            SendMsg(ERROR_INVALID_PARAMETER,"Phone is not an 11-digit normal one");
            return;
        }
        for(int i=0;i<11;i++)
            if((tmp.at(i).toLatin1()>'9')||(tmp.at(i).toLatin1()<'0')){
                SendMsg(ERROR_INVALID_PARAMETER,"Phone is not an 11-digit normal one");
                return;
            }
    }
    if(mode==1){
        if((!obj.contains("x"))||(!obj.contains("x"))){
            SendMsg(ERROR_LACK_PARAMETERS,"Lack position of the hotel.");
            return;
        }
        double x,y;
        x=obj.value("x").toDouble((double)-1);
        y=obj.value("y").toDouble((double)-1);
        if((x<0)||(y<0)){
            SendMsg(ERROR_LACK_PARAMETERS,"Lack position of the hotel.");
            return;
        }
        if(!obj.contains("desc")){
            SendMsg(ERROR_LACK_PARAMETERS,"Lack description of the hotel.");
            return;
        }
        if(!obj.contains("city")){
            SendMsg(ERROR_LACK_PARAMETERS,"Lack description of the hotel.");
            return;
        }

        if(obj.value("city").toInt(-1)<0){
            SendMsg(ERROR_LACK_PARAMETERS,"Lack city.");
            return;
        }
        int rtn=DBManager::AddHotel(obj.value("username").toString(),obj.value("phone").toString(),x,y,obj.value("city").toInt(-1),obj.value("pass").toString(),obj.value("desc").toString());
        if(rtn==ERROR_USER_EXISTS){
            SendMsg(ERROR_USER_EXISTS,"User exists.");
            return;
        }
        if(rtn==ERROR_UNKNOWN_ERROR){
            SendMsg(ERROR_UNKNOWN_ERROR,"An internal error happens.Please help contact the administrator.");
            return;
        }
        SendMsg(rtn,"New user id:"+QString::number(rtn));
        return;
    }
    v_code=(unsigned int)rand()%10000;
    std::cout<<"Captcha code:"<<v_code<<std::endl;
    try{
        int rtn=DBManager::SendCode(v_code,obj.value("phone").toString());
        if(rtn!=ERROR_NO_ERROR){
            SendMsg(ERROR_UNKNOWN_ERROR,"Verify code service offline...Therefore register is shut down.");
            return;
        }
    }catch(...){
        SendMsg(ERROR_UNKNOWN_ERROR,"Verify code service offline...Therefore register is shut down.");
        return;
    }
    history.empty();
    history.insert("username",obj.value("username").toString());
    history.insert("pass",obj.value("pass").toString());
    history.insert("phone",obj.value("phone").toString());
    SendMsg(ERROR_NO_ERROR,"OK. Continue.");
    return;
}
void Session::confirm(QJsonObject obj){
    if(!obj.contains("vcode")){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack verify code.");
        return;
    }
    if(v_code==99999){
        SendMsg(ERROR_STATE_NOT_READY,"Get verify code first.");
        return;
    }
    if((int)v_code==obj.value("vcode").toInt(-1)){
        int rtn=DBManager::AddUser(history.value("username").toString(),history.value("phone").toString(),history.value("pass").toString());
        if(rtn==ERROR_USER_EXISTS){
            SendMsg(ERROR_USER_EXISTS,"User exists.");
            return;
        }
        if(rtn==ERROR_UNKNOWN_ERROR){
            SendMsg(ERROR_UNKNOWN_ERROR,"An internal error occurs.Please help contact the administrator.");
            return;
        }
        SendMsg(ERROR_NO_ERROR,"New user id:"+QString::number(rtn));
        v_code=99999;
        return;
    }else{
        SendMsg(ERROR_WRONG_CAPTCHA,"Verify code does not match.");
        v_code=99999;
        return;
    }
}
void Session::user(QJsonObject obj){
    if(!obj.contains("type")){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack user type");
        return;
    }
    int mode=obj.value("type").toInt(-1);
    if((mode!=0)&&(mode!=1)){
        SendMsg(ERROR_LACK_PARAMETERS,"Lack user type");
        return;
    }
    if(!obj.contains("username")){
        SendMsg(ERROR_LACK_PARAMETERS,"No username provided.");
        return;
    }
    QByteArray pass;
    QString name=obj.value("username").toString();
    int TmpUser;
    if(mode==0)TmpUser=DBManager::ReadKey(name,pass);
    if(mode==1)TmpUser=DBManager::ReadHotKey(name,pass);
    if(TmpUser<0){
        if(TmpUser==ERROR_NO_SUCH_USER){
            SendMsg(ERROR_NO_SUCH_USER,"No such user.");
            return;
        }else{
            SendMsg(ERROR_UNKNOWN_ERROR,"An unknown error occurred. Please help contact the administrator.");
            return;
        }
    }
    hist.empty();
    hist.insert("mode",mode);
    hist.insert("id",TmpUser);
    _512 pwd;
    pwd=pass;
    if(password!=0)
        delete password;
    password=new PASSWORD();
    *password=pwd;
    _512 salt=password->GetSalt();
    SendMsg(ERROR_NO_ERROR,QString((QByteArray)salt));
    return;
}
void Session::pass(QJsonObject obj){
    if(password==0){
        SendMsg(ERROR_STATE_NOT_READY,"Use user command first.");
        return;
    }
    if(!obj.contains("pass")){
        SendMsg(ERROR_LACK_PARAMETERS,"No password given.");
        return;
    }
    QByteArray tmp=obj.value("pass").toString().toLatin1();
    _512 key;
    key=tmp;
    if((*password)==key){
        if(hist.value("mode").toInt(-1)==0)
        {
            SendMsg(ERROR_NO_ERROR,"Login successful as user"+QString::number(hist.value("id").toInt()));
            usr=new Customer(this);
            usr->id=hist.value("id").toInt();
        }
        else if(hist.value("mode").toInt(-1)==1)
        {
            SendMsg(ERROR_NO_ERROR,"Login successful as hotel:"+QString::number(hist.value("id").toInt()));
            usr=new Hotel(this);
            usr->id=hist.value("id").toInt();
        }
        else
        {
            SendMsg(ERROR_UNKNOWN_ERROR,"An unknown error occurred. Please help contact the administrator.");
            return;
        }
        return;
    }else{
        SendMsg(ERROR_WRONG_PASSWORD,"Wrong password.");
        delete password;
        password=0;
        return;
    }
}

void Session::do_switch(){
    try{
        QByteArray data;
        try{
            data=this->socket->readAll();
        }catch(...){
            return;
        }
        qDebug()<<data;
        QJsonDocument doc=QJsonDocument::fromJson(data);
        if(!doc.isObject()){
            SendMsg(ERROR_NO_SUCH_COMMAND,"Pardon?");
            return;
        }
        QJsonObject obj=doc.object();
        if(!obj.contains("action")){
            SendMsg(ERROR_NO_SUCH_COMMAND,"Pardon?");
            return;
        }
        QString action=obj.value("action").toString();
        if(action=="reg"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            reg(obj);
        }
        else if(action=="confirm"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            confirm(obj);
        }
        else if(action=="user"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            user(obj);
        }else if(action=="admlogin"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            admlogin(obj);
        }else if(action=="admgetall"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            admgetall(obj);
        }else if(action=="admvalid"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            admvalid(obj);
        }
        else if(action=="pass"){
            if(usr!=0){
                SendMsg(ERROR_STATE_NOT_READY,"You have already logged in");
                return;
            }
            pass(obj);

        }else if(action=="logout"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->logout();
        }else if(action=="chkey"){
            printf("%p",usr);
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->chkey(obj);
        }else if(action=="all_hotel"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->all_hotel(obj);
        }else if(action=="all_room"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->all_room(obj);
        }else if(action=="book"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->book(obj);
        }else if(action=="refund"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->refund(obj);
        }else if(action=="getimage"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
        }else if(action=="chroom"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->chroom(obj);
        }else if(action=="newroom"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->newroom(obj);
        }else if(action=="changedescription"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->changedescription(obj);
        }else if(action=="changetel"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->changetel(obj);
        }else if(action=="confirm_refund"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->confirm_refund(obj);
        }else if(action=="confirm_hotel"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->confirm_hotel(obj);
        }else if(action=="review"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->review(obj);
        }else if(action=="check_pay"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->check_pay(obj);
        }else if(action=="alltrades"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->alltrades(obj);
        }else if(action=="mydetail"){
            if(usr==0){
                SendMsg(ERROR_STATE_NOT_READY,"You have not logged in");
                return;
            }
            usr->mydetail(obj);
        }
        else{
            SendMsg(ERROR_NO_SUCH_COMMAND,"Pardon?");
            socket->readAll();
        }
    }catch(...){
        socket->readAll();
    }
}
User::User(Session *parent){
    m_parent=parent;
}
User::~User(){

    m_parent=0;
    id=0;
}
bool User::alltrades(QJsonObject ){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::logout(){
    m_parent->SendMsg(ERROR_NO_ERROR,"Bye~");
    delete m_parent;
    return true;
}
bool User::chkey(QJsonObject obj){
    if(!obj.contains("pass")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Lack password");
        return false;
    }
    QString tmp=obj.value("pass").toString();
    if(tmp.length()<128){
        m_parent->SendMsg(ERROR_INVALID_PARAMETER,"Password error");
        return false;
    }
    for(int i=0;i<128;i++){
        char ch=tmp.toStdWString().c_str()[i];
        if(((ch<'0')||(ch>'9'))&&((ch<'A')||(ch>'F'))){
            m_parent->SendMsg(ERROR_INVALID_PARAMETER,"Password error");
            return false;
        }
    }
    if(obj.value("pass").toString().length()<128){
        m_parent->SendMsg(ERROR_INVALID_PARAMETER,"Password error");
        return false;
    }
    return true;
}
bool User::all_hotel(QJsonObject obj){
    QJsonArray hotels;
    if(!obj.contains("city")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"City not determined.");
        return false;
    }
    int tmp=obj.value("city").toInt(-1);
    if(tmp<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"City not determined.");
        return false;
    }
    int rtn=DBManager::Read_Hotels(tmp,hotels);
    if(rtn!=ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_SUCH_USER,"Maybe hotel not found..");
        return false;
    }
    QJsonDocument doc(hotels);
    m_parent->SendMsg(ERROR_NO_ERROR,QString(doc.toJson()));
    return true;
}
bool User::review(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::chroom(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::check_pay(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::newroom(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::changedescription(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::changetel(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::book(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::refund(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::all_room(QJsonObject obj){
    QJsonArray rooms;
    if(!obj.contains("hotel")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Parent not determined.");
        return false;
    }
    int tmp=obj.value("hotel").toInt(-2);
    if(tmp<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Parent not determined.");
        return false;
    }
    int rtn=DBManager::Read_Rooms(tmp,rooms);
    if(rtn!=ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_SUCH_USER,"Maybe hotel not found..");
        return false;
    }
    QJsonDocument doc(rooms);
    m_parent->SendMsg(ERROR_NO_ERROR,QString(doc.toJson()));
    return true;
}
bool Hotel::all_room(QJsonObject){
    QJsonArray rooms;
    int rtn=DBManager::Read_Rooms(id,rooms);
    if(rtn!=ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_SUCH_USER,"Maybe hotel not found..");
        return false;
    }
    QJsonDocument doc(rooms);
    m_parent->SendMsg(ERROR_NO_ERROR,QString(doc.toJson()));
    return true;
}
bool User::confirm_refund(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool User::mydetail(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool Hotel::mydetail(QJsonObject){
    QJsonObject obj;
    int rtn=DBManager::Read_Hotel(id,obj);

    if(rtn!=ERROR_NO_ERROR){
        m_parent->SendMsg(rtn,"Unknown Error");
        return false;
    }
    QJsonDocument doc(obj);
    QByteArray reply=doc.toJson();
    m_parent->SendMsg(ERROR_NO_ERROR,reply);
    return false;
}
bool User::confirm_hotel(QJsonObject){
    m_parent->SendMsg(ERROR_NO_SUCH_COMMAND,"No such command");
    return false;
}
bool Customer::check_pay(QJsonObject){
    QString Self=QCoreApplication::applicationFilePath();
    QStringList arguments;
    arguments <<"void "<< *iid;
    QProcess myProcess;
    myProcess.start(Self,arguments);
    myProcess.waitForReadyRead();
    QByteArray arr=myProcess.readAllStandardOutput();
    myProcess.kill();
    if(QString(arr)=="Wait"){
        m_parent->SendMsg(ERROR_STATE_NOT_READY,"Wait...");
        return true;
    }else
    if(QString(arr)=="Good"){
        int rtn=DBManager::Pay(*tradeid);
        if(rtn==ERROR_NO_ERROR){
            m_parent->SendMsg(ERROR_NO_ERROR,"Good!");
            return true;
        }else{
            m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Horrible Mistake.Wait patiently for a refund.");
            return false;
        }
    }else{
        m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Error!");
        return false;
    }
}
bool Customer::chkey(QJsonObject obj){
    if(User::chkey(obj)){
        if(ERROR_NO_ERROR==DBManager::ChKey(id,obj.value("pass").toString().toLatin1())){
            m_parent->SendMsg(ERROR_NO_ERROR,"OK");
            return true;
        }else{
            m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Error");
            return false;
        }
    }
    else
        return false;
}
bool Hotel::chkey(QJsonObject obj){
    if(User::chkey(obj)){
        if(ERROR_NO_ERROR==DBManager::ChHotKey(id,obj.value("pass").toString().toLatin1())){
            m_parent->SendMsg(ERROR_NO_ERROR,"OK");
            return true;
        }else{
            m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Error");
            return false;
        }
    }
    else
        return false;
}
bool Customer::book(QJsonObject obj){
    if(!obj.contains("id")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No room id.");
        return false;
    }

        int roomid=obj.value("id").toInt(-1);
        if(roomid<0){
            m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No room id.");
            return false;
        }

    if(!obj.contains("start")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No start date.");
        return false;
    }
    {
        int tmp=obj.value("start").toInt(-1);
        if(tmp<0){
            m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No start date.");
            return false;
        }
    }
    if(!obj.contains("finish")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No finish date.");
        return false;
    }
    {
        int tmp=obj.value("finish").toInt(-1);
        if(tmp<0){
            m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No finish date.");
            return false;
        }
    }
    int rtn=DBManager::Book(id,roomid,obj.value("start").toInt(),obj.value("finish").toInt(),tradeid);
    if(rtn<0){
        if(ERROR_NO_EMPTY_ROOM==rtn){
            m_parent->SendMsg(ERROR_NO_SUCH_USER,"No such room.");
            return false;
        }else{
            m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"An unknown error occurred.");
            return false;
        }
    }
    QString Self=QCoreApplication::applicationFilePath();
    QStringList arguments;
    arguments << QString::number(rtn);
    QProcess myProcess;
    myProcess.start(Self,arguments);
    myProcess.waitForReadyRead();
    QByteArray arr=myProcess.readAllStandardOutput();
    myProcess.kill();
    QJsonDocument doc=QJsonDocument::fromJson(arr);
    if(!doc.isObject()){
        m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Terrible");
        return true;
    }
    QJsonObject jsobj=doc.object();
    m_parent->SendMsg(ERROR_NO_ERROR,jsobj.value("data").toString());
    *iid=jsobj.value("iid").toString();
    return true;
}
bool Customer::alltrades(QJsonObject){
    QJsonArray trades;
    int rtn=DBManager::Read_TradesByUser(id,trades);

    if(rtn!=ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_SUCH_USER,"Maybe not found..");
        return false;
    }
    QJsonDocument doc(trades);
    m_parent->SendMsg(ERROR_NO_ERROR,QString(doc.toJson()));
    return true;
}
bool Hotel::alltrades(QJsonObject){
    QJsonArray trades;
    int rtn=DBManager::Read_TradesByHotel(id,trades);

    if(rtn!=ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_SUCH_USER,"Maybe not found..");
        return false;
    }
    QJsonDocument doc(trades);
    m_parent->SendMsg(ERROR_NO_ERROR,QString(doc.toJson()));
    return true;
}
bool Customer::refund(QJsonObject obj){
    if(!obj.contains("tradeid")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Please provide trade id.");
        return false;
    }
    int tradeid=obj.value("tradeid").toInt(-1);
    if(tradeid<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Please provide trade id.");
        return false;
    }
    int rtn=DBManager::Refund(tradeid,id);
    if(rtn==ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");
        return true;
    }
    if(rtn==ERROR_STATE_NOT_READY){
        m_parent->SendMsg(ERROR_STATE_NOT_READY,"State not ready.");
        return false;
    }
    m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Something occurred.");
    return false;
}
bool Customer::review(QJsonObject obj){
    if(!obj.contains("tradeid")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which trade?");
        return false;
    }
    if(!obj.contains("star")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which rate?");
        return false;
    }
    int trade=obj.take("tradeid").toInt(-1);
    int star=obj.take("star").toInt(-1);
    if(trade<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which trade?");
        return false;
    }
    if(star<=0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which rate?");
        return false;
    }
    int rtn=DBManager::Star(trade,star);
    if(rtn==ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");
        return true;
    }
    if(rtn==ERROR_STATE_NOT_READY){
        m_parent->SendMsg(ERROR_STATE_NOT_READY,"State not ready.");
        return false;
    }
    m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Something occurred.");
    return false;
}
bool Hotel::chroom(QJsonObject obj){
    if(!obj.contains("roomid")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which room?");
        return false;
    }
    int roomid=obj.value("roomid").toInt(-1);
    if(roomid<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which room?");
        return false;
    }
    if(!obj.contains("type")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    if(!obj.contains("type")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    if(!obj.contains("name")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    if(!obj.contains("price")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    int price=obj.value("price").toInt(-1);
    if(price<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"How much?");
        return false;
    }
    if(!obj.contains("number")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"How many?");
        return false;
    }
    int number=obj.value("number").toInt(-1);
    if(number<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"How many?");
        return false;
    }
    int rtn=DBManager::ChangeRoom(roomid,id,obj.value("type").toString(),obj.value("name").toString(),price,number);
    if(rtn>=0){
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");
        return true;
    }
    else{
        m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Bad");
        return false;
    }
}
bool Hotel::newroom(QJsonObject obj){
    if(!obj.contains("type")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    if(!obj.contains("type")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    if(!obj.contains("name")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    if(!obj.contains("price")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"Which type?");
        return false;
    }
    int price=obj.value("price").toInt(-1);
    if(price<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"How much?");
        return false;
    }
    if(!obj.contains("number")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"How many?");
        return false;
    }
    int number=obj.value("number").toInt(-1);
    if(number<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"How many?");
        return false;
    }

    int rtn=DBManager::NewRoom(id,obj.value("type").toString(),obj.value("name").toString(),price,number);
    if(rtn>=0){
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");
        return true;
    }
    else{
        m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Bad");
        return false;
    }
}

bool Hotel::changedescription(QJsonObject obj){
    if(!obj.contains("desc")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No description?");
        return false;
    }
    int rtn=DBManager::ChHotDesc(id,obj.value("desc").toString());
    if(rtn)
        m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Unknown error");
    else
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");

    return true;
}
bool Hotel::changetel(QJsonObject obj){
    if(!obj.contains("tel")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No tel?");
        return false;
    }
    int rtn=DBManager::ChHotTel(id,obj.value("tel").toString());
    if(rtn)
        m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Unknown error");
    else
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");
    return true;
}
bool Hotel::confirm_refund(QJsonObject obj){
    if(!obj.contains("tradeid")){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No trade id?");
        return false;
    }
    int tradeid=obj.value("tradeid").toInt(-1);
    if(tradeid<0){
        m_parent->SendMsg(ERROR_LACK_PARAMETERS,"No trade id?");
        return false;
    }
    int rtn=DBManager::Verify_Refund(tradeid,id);
    if(rtn==ERROR_NO_ERROR){
        m_parent->SendMsg(ERROR_NO_ERROR,"OK");
        return true;
    }else
    if(rtn==ERROR_STATE_NOT_READY){
        m_parent->SendMsg(ERROR_STATE_NOT_READY,"Not in the correct state.");
        return false;
    }
    m_parent->SendMsg(ERROR_UNKNOWN_ERROR,"Unknown error.");
    return false;
}
void Session::admlogin(QJsonObject obj){
    if(!obj.contains("username")){
        SendMsg(ERROR_LACK_PARAMETERS,"Username.");
        return;
    }
    if(!obj.contains("password")){
        SendMsg(ERROR_LACK_PARAMETERS,"Password.");
        return;
    }
    HANDLE h;
    int rtn=LogonUser(obj.value("username").toString().toStdWString().c_str(),L".",obj.value("password").toString().toStdWString().c_str(),LOGON32_LOGON_NETWORK,LOGON32_PROVIDER_DEFAULT,&h);
    if(rtn){
        SendMsg(ERROR_NO_ERROR,"OK.");
        v_code=81599; //Code for Admin
        return;
    }else{
        SendMsg(ERROR_WRONG_PASSWORD,"Wrong Password.");
        return;
    }
}
void Session::admgetall(QJsonObject obj){
    if(v_code!=81599){
        SendMsg(ERROR_NO_SUCH_COMMAND,"Pardon?");
        return;
    }
    QJsonArray array;
    DBManager::ToValid(array);
    QJsonDocument doc(array);
    SendMsg(ERROR_NO_ERROR,doc.toJson());
    return;
}
void Session::admvalid(QJsonObject obj){
    if(v_code!=81599){
        SendMsg(ERROR_NO_SUCH_COMMAND,"Pardon?");
        return;
    }
    if(!obj.contains("id")){
        SendMsg(ERROR_LACK_PARAMETERS,"id?");
        return;
    }
    int hid=obj.value("id").toInt(-1);
    if(hid<0){
        SendMsg(ERROR_LACK_PARAMETERS,"id?");
        return;
    }
    DBManager::Valid(hid);
    SendMsg(ERROR_NO_ERROR,"OK");
    return;
}
