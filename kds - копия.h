#ifndef KDS_H
#define KDS_H

#include "comport.h"

using namespace std;

class KDS :public COMPORT {
    Q_OBJECT
public:
    KDS();
    static uint CalcCrc(QByteArray& parcel);
    static uint CalcCrc(const char* parcel);

    int getDev(int addr);

    int getSerNum() const;
    void setSerNum(int value);

    int getAddress() const;
    void setAddress(int value);

    int getChMum() const;
    void setChMum(int value);

    int getBaudRate() const;
    void setBaudRate(int value);

    int getOutPin() const;
    void setOutPin(int value);

    bool setOut(int addr, int value);

private:
    uint getUintData(vector<char> data, bool* ok = 0);
    bool getSuccess(vector<char> data);

    uint dev;
    int serNum;
    int address;
    int chMum;
    int baudRate;
    int outPin;
    int out;
};

#endif // KDS_H
