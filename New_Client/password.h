#ifndef PASSWORD_H
#define PASSWORD_H
#include<QByteArray>
class _512{
private:
    const char Hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    unsigned char data[64];
    unsigned int Num(char hex);
    void LoadFromHex(const QByteArray &rval);
    QByteArray ConvertToHex();
    QByteArray ToRaw();
public:
    void FromRaw(const QByteArray &rval);
    _512& operator =(const _512 &rval);
    _512& operator =(const QByteArray &rval);
    operator QByteArray();
    _512& operator =(const unsigned int &rval);
    bool operator ==(const _512 &rval) const;
    _512 operator ^(const _512 &rval);

    _512 hash();
};

class PASSWORD{
private:
    _512 salt;
    _512 pass;
    bool valid;
public:
    PASSWORD();
    ~PASSWORD();
    _512 WithSalt(_512 &rval);
    _512 GetSalt();
    PASSWORD& operator =(const _512 &rval);
    bool operator ==(const _512 &rval);
};
#endif // PASSWORD_H
