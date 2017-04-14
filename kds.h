#ifndef KDS_H
#define KDS_H

#include <QtSerialPort>

using namespace std;

class KDS : public QSerialPort {
    Q_OBJECT
public:
    KDS();
    int getDev(int addr);
    bool setOut(int addr, int value);

private:
    uint getUintData(QByteArray data);
    bool getSuccess(QByteArray data);
    QString CalcCrc(QByteArray& parcel);

    int dev;
    int serNum;
    int address;
    int chMum;
    int baudRate;
    int outPin;
    int out;
};

#endif // KDS_H
