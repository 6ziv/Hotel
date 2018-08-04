#define QT_NO_DEBUG

#include "dbmanager.h"
#include<QSqlError>
QSqlDatabase DBManager::db;         //程序主数据库
QMutex DBManager::m_db;             //适时加锁，防止逻辑错误
QMutex DBManager::m_cell;             //适时加锁，防止逻辑错误
QTcpSocket DBManager::socket;       //与手机通信（验证码）
QMap<int,HotelInfo> DBManager::hotels;
QMap<int,RoomInfo> DBManager::rooms;
QVector<Hotels_In_A_City> DBManager::C_H;


void DBManager::init()              //初始化数据库，以及其它杂项

{
    std::cout<<"Connecting to cell...";
    socket.connectToHost("localhost",6000);
    socket.write("ping\n");
    socket.flush();
    if(socket.waitForReadyRead(5000)){
        std::cout<<"Done"<<std::endl;
    }else
        std::cout<<"Error"<<std::endl;


    hotels.clear();
    std::cout<<"Opening Database...";
    db= QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("Data.db");

    bool ok = db.open();
    if(!ok){
        std::cout<<"Error!"<<std::endl;
        return;
    }
    QSqlQuery query;

    //普通用户
    ok=query.exec("CREATE TABLE IF NOT EXISTS USERS ("
                  "Username VARCHAR(128),"
                  "Phone VARCHAR(16),"
                  "Pass VARCHAR(256),"
                  "ID INTEGER PRIMARY KEY   AUTOINCREMENT);");

    //酒店信息及管理账号
    ok=ok&&query.exec("CREATE TABLE IF NOT EXISTS HOTELS ("
                  "Name VARCHAR(128),"
                  "Tel VARCHAR(32),"
                  "X DOUBLE,"
                  "Y DOUBLE,"
                  "Desc VARCHAR(1024),"
                  "City INT UNSIGNED,"
                  "Pass VARCHAR(256),"
                  "Valid BOOLEAN,"
                  "ID INTEGER PRIMARY KEY AUTOINCREMENT);");

    //房间信息
    ok=ok&&query.exec("CREATE TABLE IF NOT EXISTS ROOMS ("
                  "Desc VARCHAR(1024),"
                  "Type VARCHAR(128),"
                  "Parent INT UNSIGNED,"
                  "Price INT UNSIGNED,"
                  "Number INT UNSIGNED,"
                  "ID INTEGER PRIMARY KEY   AUTOINCREMENT);");

    //订单信息
    ok=ok&&query.exec("CREATE TABLE IF NOT EXISTS TRADES ("
                  "Desc VARCHAR(128),"
                  "Customer INT UNSIGNED,"
                  "Hotel INT UNSIGNED,"
                  "Room INT UNSIGNED,"
                  "Price INT UNSIGNED,"
                  "Start INT UNSIGNED,"
                  "State INT UNSIGNED,"
                  "Finish INT UNSIGNED,"
                  "Star INT UNSIGNED,"
                  "ID INTEGER PRIMARY KEY   AUTOINCREMENT);");


    if(!ok){
        std::cout<<"error!"<<query.lastError().text().toStdString()<<std::endl;
        return;
    }
    std::cout<<"OK!"<<std::endl;
    std::cout<<"Loading data...";
    for(int i=0;i<6;i++){
        Hotels_In_A_City tmp;
        tmp.clear();
        C_H.push_back(tmp);
    }
    ok=query.exec("SELECT * FROM HOTELS WHERE Valid=1;");
    if(!ok){
        std::cout<<"error!"<<query.lastError().text().toStdString()<<std::endl;
        return;
    }
    query.next();
    while(query.isValid()){
        HotelInfo tmp;
        tmp.city=query.value("City").toInt();
        tmp.desc=query.value("Desc").toString();
        tmp.id=query.value("ID").toInt();
        tmp.lowest=2147483647;
        tmp.name=query.value("Name").toString();
        tmp.rooms.clear();
        tmp.starred=0;
        tmp.total=0;
        tmp.tel=query.value("Tel").toString();
        tmp.x=query.value("X").toDouble();
        tmp.y=query.value("Y").toDouble();
        hotels.insert(tmp.id,tmp);
        std::cout<<"inserting "<<tmp.id<<std::endl;
        C_H[tmp.city].push_back(tmp.id);
        query.next();
    }


    ok=query.exec("SELECT * FROM ROOMS;");
    if(!ok){
        std::cout<<"error!"<<query.lastError().text().toStdString()<<std::endl;
        return;
    }
    query.next();
    {
        RoomInfo tmp;
        while(query.isValid()){
            tmp.desc=query.value("Desc").toString();
            tmp.id=query.value("id").toInt();
            tmp.number=query.value("number").toInt();
            tmp.parent=query.value("Parent").toInt();
            tmp.price=query.value("Price").toInt();
            tmp.type=query.value("type").toString();
            rooms.insert(tmp.id,tmp);
            QMap<int,HotelInfo>::iterator iter=hotels.find(tmp.parent);
            if(iter!=hotels.end())
                (*iter).rooms.push_back(tmp.id);
            if(iter->lowest>tmp.price)iter->lowest=tmp.price;
            query.next();
        }
    }

    ok=query.exec("SELECT * FROM TRADES;");
    if(!ok){
        std::cout<<"error!"<<query.lastError().text().toStdString()<<std::endl;
        return;
    }
    query.next();
    while(query.isValid()){
        if(query.value("Star").toInt()!=0){
            QMap<int,HotelInfo>::iterator iter=hotels.find(query.value("Hotel").toInt());
            iter->starred++;
            iter->total+=query.value("Star").toInt();
        }
        query.next();
    }

    std::cout<<"done!"<<std::endl;
    std::cout<<"Refreshing pay requests.";
    ok=query.exec("UPDATE TRADES SET State=14 WHERE State=10;");
    if(ok)std::cout<<"done!"<<std::endl;
    else std::cout<<"error!"<<std::endl;
}

//判断（普通）用户存在性
int DBManager::IsUser(QString username, QString phone){
    QSqlQuery query;
    query.prepare("SELECT * FROM USERS WHERE Username=:uname OR Phone=:phon;");
    query.bindValue(":uname",username);
    query.bindValue(":phon",phone);
    bool ok=query.exec();
    if(!ok){
        std::cout<<"SQL Error!"<<std::endl;
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(!query.isValid())
        return ERROR_NO_SUCH_USER;
    else
        return ERROR_USER_EXISTS;
}

//添加普通用户
int DBManager::AddUser(QString username,  QString phone,QString password){
    m_db.lock();
    if(ERROR_NO_SUCH_USER==IsUser(username,phone)){
        QSqlQuery query;
        query.prepare("INSERT INTO Users(Username,Phone,Pass) values(:uname,:phon,:pass)");
        query.bindValue(":uname",username);
        query.bindValue(":phon",phone);
        query.bindValue(":pass",password);
        query.exec();
        m_db.unlock();
        query.prepare("SELECT * FROM USERS WHERE Username=:uname;");
        query.bindValue(":uname",username);
        query.exec();
        query.next();
        if(!query.isValid())return ERROR_NO_SUCH_USER;
        return query.value("ID").toInt();
    }else{
        m_db.unlock();
        return ERROR_USER_EXISTS;
    }
}

//判断酒店存在性
int DBManager::IsHotel(QString name){
    for(QMap<int,HotelInfo>::ConstIterator iter=hotels.begin();iter!=hotels.begin();iter++){
        if(iter->name==name)return ERROR_USER_EXISTS;
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM HOTELS WHERE Name=:name;");
    query.bindValue(":name",name);
    bool ok=query.exec();
    if(!ok){
        std::cout<<"SQL Error!"<<std::endl;
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(!query.isValid())
        return ERROR_NO_SUCH_USER;
    else
        return ERROR_USER_EXISTS;
}

//新增酒店
int DBManager::AddHotel(QString name,QString tel, double x,double y,int city ,QString password,QString description){
    qDebug()<<city;
    if((city<0)||(city>5))return ERROR_INVALID_PARAMETER;
    m_db.lock();
    if(ERROR_NO_SUCH_USER==IsHotel(name)){
        QSqlQuery query;
        query.prepare("INSERT INTO HOTELS(Name,Tel,X,Y,City,Pass,Desc,Valid) values(:name,:tel,:x,:y,:city,:pass,:desc,:valid)");
        query.bindValue(":name",name);
        query.bindValue(":tel",tel);
        query.bindValue(":x",x);
        query.bindValue(":y",y);
        query.bindValue(":city",city);
        query.bindValue(":pass",password);
        query.bindValue(":desc",description);
        query.bindValue(":valid",false);
        query.exec();
        m_db.unlock();
        query.prepare("SELECT * FROM HOTELS WHERE Name=:name;");
        query.bindValue(":name",name);
        query.exec();
        query.next();
        if(!query.isValid())return ERROR_NO_SUCH_USER;
        return query.value("ID").toInt();

    }else{
        m_db.unlock();
        return ERROR_USER_EXISTS;
    }
}

//读取普通用户密码（登陆用）
int DBManager::ReadKey(QString username,QByteArray& password){
    QSqlQuery query;
    query.prepare("SELECT * FROM USERS WHERE Username=:uname OR Phone=:phon;");
    query.bindValue(":uname",username);
    query.bindValue(":phon",username);
    bool ok=query.exec();
    if(!ok){
        std::cout<<"SQL Error!"<<std::endl;
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }

    query.next();
    if(!query.isValid())
        return ERROR_NO_SUCH_USER;
    QByteArray Hex=query.value("Pass").toByteArray();
    password=Hex;
    return query.value("ID").toInt();
}

//读取酒店密码（登陆用）
int DBManager::ReadHotKey(QString username,QByteArray& password){
    QSqlQuery query;
    query.prepare("SELECT * FROM HOTELS WHERE (Name=:uname AND Valid=1);");
    query.bindValue(":uname",username);
    bool ok=query.exec();
    if(!ok){
        std::cout<<"SQL Error!"<<std::endl;
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }

    query.next();
    if(!query.isValid())
        return ERROR_NO_SUCH_USER;
    QByteArray Hex=query.value("Pass").toByteArray();
    password=Hex;
    return query.value("ID").toInt();
}

//更换密码
int DBManager::ChKey(int id,QByteArray password){

    QSqlQuery query;
    query.prepare("UPDATE USERS SET Pass=:pass WHERE ID=:id;");
    query.bindValue(":id",id);
    query.bindValue(":pass",password);
    bool ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    return ERROR_NO_ERROR;
}

//更改酒店密码
int DBManager::ChHotKey(int id,QByteArray password){
    QSqlQuery query;
    query.prepare("UPDATE HOTELS SET Pass=:pass WHERE ID=:id;");
    query.bindValue(":id",id);
    query.bindValue(":pass",password);
    bool ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    return ERROR_NO_ERROR;
}

//更改酒店描述
int DBManager::ChHotDesc(int id,QString desc){
    QSqlQuery query;
    query.prepare("UPDATE HOTELS SET Desc=:desc WHERE ID=:id;");
    query.bindValue(":id",id);
    query.bindValue(":desc",desc);
    bool ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    QMap<int,HotelInfo>::iterator iter=hotels.find(id);
    iter->desc=desc;
    return ERROR_NO_ERROR;
}

//更改酒店电话
int DBManager::ChHotTel(int id,QString tel){
    QSqlQuery query;
    query.prepare("UPDATE HOTELS SET Tel=:tel WHERE ID=:id;");
    query.bindValue(":id",id);
    query.bindValue(":tel",tel);
    bool ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    QMap<int,HotelInfo>::iterator iter=hotels.find(id);
    iter->tel=tel;
    return ERROR_NO_ERROR;
}

//新建房型
int DBManager::NewRoom(int parent,QString type,QString name,int price,int number){
    QSqlQuery query;
    query.prepare("INSERT INTO ROOMS (Desc,Type,Parent,Price,Number) values(:desc,:type,:parent,:price,:number);");
    query.bindValue(":desc",name);
    query.bindValue(":type",type);
    query.bindValue(":parent",parent);
    query.bindValue(":price",price);
    query.bindValue(":number",number);
    bool ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    int id=query.lastInsertId().toInt();
    RoomInfo tmp;
    tmp.id=id;
    tmp.desc=name;
    tmp.number=number;
    tmp.parent=parent;
    tmp.price=price;
    tmp.type=type;
    rooms.insert(tmp.id,tmp);
    QMap<int,HotelInfo>::iterator iter=hotels.find(tmp.parent);
    iter->rooms.push_back(tmp.id);
    if(iter->lowest>tmp.price)iter->lowest=tmp.price;
    return ERROR_NO_ERROR;
}

//更改信息
int DBManager::ChangeRoom(int id,int parent,QString type,QString name,int price,int number){
    QSqlQuery query;
    query.prepare("SELECT * FROM ROOMS WHERE ID=:id;");
    query.bindValue(":id",id);
    bool ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(!query.isValid())return ERROR_NO_SUCH_USER;
    if(query.value("Parent").toInt()!=parent)return ERROR_ACCESS_DENIED;
    query.prepare("UPDATE ROOMS SET Type=:type,Desc=:desc,Price=:price,Number=:number WHERE ID=:id;");
    query.bindValue(":type",type);
    query.bindValue(":desc",name);
    query.bindValue(":price",price);
    query.bindValue(":number",number);
    query.bindValue(":id",id);

    ok=query.exec();
    if(!ok){
        return ERROR_UNKNOWN_ERROR;
    }
    QMap<int,RoomInfo>::iterator iter1=rooms.find(id);
    iter1->desc=name;
    iter1->number=number;
    iter1->price=price;
    iter1->type=type;
    QMap<int,HotelInfo>::iterator iter2=hotels.find(iter1->parent);
    if(iter2->lowest>iter1->price)iter2->lowest=iter1->price;
    return ERROR_NO_ERROR;
}


int DBManager::ToValid(QJsonArray &array){
    array.empty();

    QSqlQuery query;
    query.prepare("SELECT * FROM HOTELS WHERE Valid=0;");
    query.exec();
    query.next();
    while(query.isValid()){
        QJsonObject obj;
        obj.empty();
        obj.insert("id",query.value("ID").toInt());
        obj.insert("name",query.value("Name").toString());
        obj.insert("tel",query.value("Tel").toString());
        array.push_back(obj);
        query.next();
    }
    return ERROR_NO_ERROR;
}

//审核通过
int DBManager::Valid(int id){
    QSqlQuery query;
    query.prepare("UPDATE HOTELS SET Valid=:valid WHERE ID=:id;");
    query.bindValue(":valid",true);
    query.bindValue(":id",id);
    bool ok=query.exec();
    query.prepare("SELECT * FROM HOTELS WHERE ID=:id;");
    query.bindValue(":id",id);
    query.exec();
    query.next();
    if(!query.isValid())return ERROR_UNKNOWN_ERROR;
    HotelInfo tmp;
    tmp.city=query.value("City").toInt();
    tmp.id=query.value("ID").toInt();
    tmp.name=query.value("Name").toString();
    tmp.rooms.clear();
    tmp.starred=0;
    tmp.tel=query.value("tel").toString();;
    tmp.total=0;
    tmp.x=query.value("X").toDouble();
    tmp.y=query.value("Y").toDouble();
    hotels.insert(tmp.id,tmp);
    C_H[tmp.city].push_back(tmp.id);
    return tmp.id;

    if(!ok){
        return ERROR_NO_SUCH_USER;
    }
    return ERROR_NO_ERROR;
}

//剩余房间数
int DBManager::hasLeft(int id,int date){
    QSqlQuery query;
    //部分SQL语句被禁止使用… query.prepare("SELECT COUNT(*) FROM TRADES WHERE Room=:id AND Start<=:start AND Finish >=:finish AND (State=10 OR State=11);");
    query.prepare("SELECT * FROM TRADES WHERE Room=:id AND Start<=:start AND Finish >=:finish AND (State=10 OR State=11);");
    query.bindValue(":id",id);
    query.bindValue(":start",date);
    query.bindValue(":finish",date);
    bool ok=query.exec();
    if(!ok){
        return ERROR_NO_SUCH_USER;
    }
    query.next();
    //int taken=query.value(0).toInt();
    int taken=query.size();
    QMap<int,RoomInfo>::iterator iter=rooms.find(id);
    if(iter==rooms.cend())
        return ERROR_NO_SUCH_USER;
    int num=iter->number;
    return num-taken;
}

//预订
int DBManager::Book(int user,int room,int start,int finish,int *tradeid){
    QMap<int,RoomInfo>::iterator iter=rooms.find(room);
    if(iter==rooms.end())return ERROR_NO_SUCH_USER;
    int hotel=iter->parent;
    int price=iter->price;
    QMap<int,HotelInfo>::iterator iter2=hotels.find(hotel);
    if(iter2==hotels.end())return ERROR_UNKNOWN_ERROR;
    QSqlQuery query;
    m_db.lock();
    int tmp;
    for(int i=start;i<=finish;i++){
        tmp=hasLeft(room,i);
        if(tmp<=0){
            m_db.unlock();
            if(tmp==0)
                return ERROR_NO_EMPTY_ROOM;
            else
                return ERROR_UNKNOWN_ERROR;
        }
    }
    QString desc=iter2->name+"-"+iter->type;

    query.prepare("INSERT INTO TRADES (Desc,Customer,Hotel,Room,Price,Start,Finish,State,Star) values(:desc,:user,:hotel,:room,:price,:start,:finish,:state,:star);");
    query.bindValue(":desc",desc);
    query.bindValue(":user",user);
    query.bindValue(":hotel",hotel);
    query.bindValue(":room",room);
    query.bindValue(":price",price);
    query.bindValue(":start",start);
    query.bindValue(":finish",finish);
    query.bindValue(":state",TRADE_NOT_PAYED);
    query.bindValue(":star",0);

    if(!query.exec()){
        m_db.unlock();
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    int last=query.lastInsertId().toInt();
    *tradeid=last;
    m_db.unlock();
    return (iter->price)*(finish-start+1);
}

int DBManager::Refund(int trade,int user){
    QSqlQuery query;

    query.prepare("SELECT * FROM TRADES WHERE ID=:id;");
    query.bindValue(":id",trade);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(query.value("Customer").toInt()!=user)return ERROR_ACCESS_DENIED;
    if(query.value("State").toInt()!=TRADE_PAYED)return ERROR_STATE_NOT_READY;
    query.prepare("UPDATE TRADES SET State=:state WHERE ID=:id;");
    query.bindValue(":id",trade);
    query.bindValue(":state",TRADE_REFUNDING);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }

    return ERROR_NO_ERROR;
}

int DBManager::Verify_Refund(int trade,int hotel){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE ID=:id;");
    query.bindValue(":id",trade);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(query.value("Hotel").toInt()!=hotel)return ERROR_ACCESS_DENIED;
    if(query.value("State").toInt()!=TRADE_REFUNDING)return ERROR_STATE_NOT_READY;
    int star=query.value("Star").toInt(0);
    query.prepare("UPDATE TRADES SET State=:state, Star=:star WHERE ID=:id;");
    query.bindValue(":id",trade);
    query.bindValue(":state",TRADE_REFUNDED);
    query.bindValue(":star",0);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    QMap<int,HotelInfo>::iterator iter=hotels.find(hotel);
    if(star){
        iter->starred--;
        iter->total-=star;
    }
    return ERROR_NO_ERROR;
}

int DBManager::Checkin(int trade,int hotel){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE ID=:id;");
    query.bindValue(":id",trade);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(query.value("Hotel").toInt()!=hotel)return ERROR_ACCESS_DENIED;
    if(query.value("State").toInt()!=TRADE_PAYED)return ERROR_STATE_NOT_READY;
    query.prepare("UPDATE TRADES SET State=:state WHERE ID=:id;");
    query.bindValue(":id",trade);
    query.bindValue(":state",TRADE_DONE);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    return ERROR_NO_ERROR;
}
int DBManager::Pay(int tradeid){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE ID=:id;");
    query.bindValue(":id",tradeid);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(!query.isValid()){
        return ERROR_UNKNOWN_ERROR;
    }
    if(query.value("State").toInt()!=TRADE_NOT_PAYED)return ERROR_STATE_NOT_READY;
    query.prepare("UPDATE TRADES SET State=:state WHERE ID=:id;");
    query.bindValue(":id",tradeid);
    query.bindValue(":state",TRADE_PAYED);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    return ERROR_NO_ERROR;
}
int DBManager::Timeout(int tradeid){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE ID=:id;");
    query.bindValue(":id",tradeid);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(query.value("State").toInt()!=TRADE_NOT_PAYED)return ERROR_STATE_NOT_READY;
    query.prepare("UPDATE TRADES SET State=:state WHERE ID=:id;");
    query.bindValue(":id",tradeid);
    query.bindValue(":state",TRADE_ABONDONED);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    return ERROR_NO_ERROR;
}
int DBManager::Star(int trade,int star){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE ID=:id;");
    query.bindValue(":id",trade);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    if(query.value("State").toInt()!=TRADE_DONE)return ERROR_STATE_NOT_READY;
    int hotel=query.value("Hotel").toInt();
    query.prepare("UPDATE TRADES SET Star=:star WHERE ID=:id;");
    query.bindValue(":id",trade);
    query.bindValue(":star",star);
    if(!query.exec()){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    QMap<int,HotelInfo>::iterator iter=hotels.find(hotel);
    iter->starred++;
    iter->total+=star;
    return ERROR_NO_ERROR;
}
int DBManager::GoodMorning(int day){
    QSqlQuery query;
    query.prepare("UPDATE TRADES SET State=15 WHERE (Start<=:day AND State=11);");
    query.bindValue(":day",day);
    query.exec();
    return ERROR_NO_ERROR;
}
int DBManager::Read_Hotels(int city,QJsonArray &array){
    array.empty();
    if((C_H.size()<=city)||(city<0))return ERROR_INVALID_PARAMETER;
    for (QVector<int>::ConstIterator iter=C_H.at(city).cbegin();iter!=C_H.at(city).cend();iter++){
        QMap<int,HotelInfo>::ConstIterator iter2=hotels.find(*iter);
        if(iter2==hotels.end()){
            continue;
        }

        QJsonObject obj;
        obj.insert("id",iter2->id);
        obj.insert("tel",iter2->tel);
        obj.insert("x",iter2->x);
        obj.insert("y",iter2->y);
        obj.insert("city",(int)iter2->city);
        obj.insert("name",iter2->name);
        obj.insert("desc",iter2->desc);
        obj.insert("lowest",(int)iter2->lowest);
        if(iter2->starred)
            obj.insert("star",(double)(iter2->total)/(iter2->starred));
        else
            obj.insert("star",(double)0);
        array.push_back(obj);
    }
    return ERROR_NO_ERROR;
}

int DBManager::Read_Hotel(int hotel,QJsonObject &obj){
        QMap<int,HotelInfo>::ConstIterator iter2=hotels.find(hotel);
        if(iter2==hotels.cend()){
            return ERROR_NO_SUCH_USER;
        }

        obj.insert("tel",iter2->tel);
        obj.insert("name",iter2->name);
        obj.insert("desc",iter2->desc);
    return ERROR_NO_ERROR;
}

int DBManager::Read_TradesByUser(int id,QJsonArray &array){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE Customer=:id;");
    query.bindValue(":id",id);
    bool ok=query.exec();
    if(!ok){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    while(query.isValid()){
        QJsonObject obj;
        obj.empty();
        obj.insert("id",query.value("ID").toInt());
        obj.insert("price",query.value("Price").toInt());
        obj.insert("start",query.value("Start").toInt());
        obj.insert("finish",query.value("Finish").toInt());
        obj.insert("state",query.value("State").toInt());
        obj.insert("desc",query.value("Desc").toString());
        obj.insert("star",query.value("Star").toInt());
        array.push_back(obj);
        query.next();
    }
    return ERROR_NO_ERROR;
}
int DBManager::Read_TradesByHotel(int id,QJsonArray &array){
    QSqlQuery query;
    query.prepare("SELECT * FROM TRADES WHERE Hotel=:id;");
    query.bindValue(":id",id);
    bool ok=query.exec();
    if(!ok){
        std::cout<<query.lastError().text().toStdString()<<std::endl;
        return ERROR_UNKNOWN_ERROR;
    }
    query.next();
    while(query.isValid()){
        QJsonObject obj;
        obj.insert("id",query.value("ID").toInt());
        obj.insert("room",query.value("Room").toInt());
        obj.insert("price",query.value("Price").toInt());
        obj.insert("start",query.value("Start").toInt());
        obj.insert("finish",query.value("Finish").toInt());
        obj.insert("state",query.value("State").toInt());
        array.push_back(obj);
        query.next();
    }
    return ERROR_NO_ERROR;
}


int DBManager::Read_Rooms(int hotel,QJsonArray &array){
    array.empty();
    QMap<int,HotelInfo>::const_iterator iter=hotels.find(hotel);
    if(iter==hotels.cend()){
        return ERROR_NO_SUCH_USER;
    }

    for (QVector<int>::ConstIterator iter2=(iter->rooms).begin();iter2!=(iter->rooms).end();iter2++){
        QMap<int,RoomInfo>::const_iterator iter3=rooms.find(*iter2);
        QJsonObject obj;
        obj.insert("id",iter3->id);
        obj.insert("type",iter3->type);
        obj.insert("desc",iter3->desc);
        obj.insert("price",(int)(iter3->price));
        obj.insert("number",(int)(iter3->number));
        array.append(obj);
    }

    return ERROR_NO_ERROR;
}

int DBManager::SendCode(int code,QString dest){
    try{
        QString msg="您的验证码为:"+QString::number(code).rightJustified(4,'0')+"。请妥善保存。【酒店管理测试】";
        QJsonObject o;
        o.empty();
        o.insert("phone",dest);
        o.insert("msg",msg);
        QByteArray QBA=QJsonDocument(o).toJson();
        QBA.replace("\n"," ");
        QBA.append("\n");
        socket.write(QBA);
        socket.flush();
        if(socket.waitForReadyRead(5000)){
            return ERROR_NO_ERROR;
        }else{
            std::cout<<"Error"<<std::endl;
            return ERROR_UNKNOWN_ERROR;
        }
    }catch(...){
        return ERROR_UNKNOWN_ERROR;
    };
}
