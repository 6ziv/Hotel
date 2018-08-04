#define QT_NO_DEBUG

#include "password.h"
#include"windows.h"
_512& _512::operator =(const _512 &rval){
    for(int i=0;i<64;i++)
        data[i]=rval.data[i];
    return *this;
}
_512& _512::operator =(const unsigned int &rval){
    srand(rval);
    for(int i=0;i<64;i++)
        data[i]=rand()%256;
    return *this;
}
_512& _512::operator =(const QByteArray &rval){
    LoadFromHex(rval);
    return *this;
}
_512::operator QByteArray(){
    return ConvertToHex();
}
bool _512::operator ==(const _512 &rval) const{
    for(int i=0;i<64;i++)
        if(rval.data[i]!=data[i])
            return false;
    return true;
}
_512 _512::operator ^(const _512 &rval){
    _512 rtn;
    rtn=*this;
    for(int i=0;i<64;i++)
        rtn.data[i]=data[i]^rval.data[i];
    return rtn;
}
unsigned int _512::Num(char hex){
    if(hex=='0')return  0;if(hex=='1')return  1;if(hex=='2')return  2;if(hex=='3')return  3;
    if(hex=='4')return  4;if(hex=='5')return  5;if(hex=='6')return  6;if(hex=='7')return  7;
    if(hex=='8')return  8;if(hex=='9')return  9;if(hex=='A')return 10;if(hex=='B')return 11;
    if(hex=='C')return 12;if(hex=='D')return 13;if(hex=='E')return 14;if(hex=='F')return 15;
    return -1;
}
void _512::LoadFromHex(const QByteArray &rval){
    for(int i=0;i<64;i++)
        data[i]=Num(rval.at(2*i))*16+Num(rval.at(2*i+1));
}
QByteArray _512::ConvertToHex(){
    QByteArray rtn;
    for(int i=0;i<64;i++){
        rtn.push_back(Hex[data[i]/16]);
        rtn.push_back(Hex[data[i]%16]);
    }
    return rtn;
}
void _512::FromRaw(const QByteArray &rval){
    for(int i=0;i<64;i++)
        data[i]=(unsigned char)rval.at(i);
}
QByteArray _512::ToRaw(){
    QByteArray rtn;
    rtn.clear();
    for(int i=0;i<64;i++)
        rtn.push_back((char)data[i]);
    return rtn;
}

_512 _512::hash(){
    QByteArray input=ToRaw();
    QByteArray output=QCryptographicHash::hash(input,QCryptographicHash::Sha3_512);
    _512 rtn;
    rtn.FromRaw(output);
    return rtn;
}
PASSWORD::PASSWORD(){
    salt=0;
    pass=0;
    valid=false;
}
PASSWORD::~PASSWORD(){
    pass=0;
    salt=0;
    valid=false;
}
_512 PASSWORD::WithSalt(_512 &rval){
    salt=rval;
    salt=salt^pass;
    salt=salt.hash();
    return salt;
}
_512 PASSWORD::GetSalt(){
    valid=true;
    salt=(unsigned int)GetTickCount();
    _512 rtn=salt;
    salt=salt^pass;
    salt=salt.hash();
    return rtn;
}
PASSWORD& PASSWORD::operator =(const _512 &rval){
    pass=rval;
    return *this;
}
bool PASSWORD::operator ==(const _512 &rval){
    if(!valid)return false;
    valid=false;
    if(salt==rval)
        return true;
    else
        return false;
}
