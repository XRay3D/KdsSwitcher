#include "kds.h"

#include <QDebug>

KDS::KDS()
{
}

uint KDS::CalcCrc(QByteArray& parcel)
{
    uint crc = 0xFFFF;
    for (int i = 1; i < parcel.size(); i++) {
        crc ^= parcel[i];
        for (int j = 0; j < 8; j++)
            (crc & 0x0001) ? crc = (crc >> 1) ^ 0xA001 : crc >>= 1;
    }
    return crc;
}

uint KDS::CalcCrc(const char* parcel)
{
    uint crc = 0xFFFF;
    int i = 0;
    while (parcel[++i] != 0) {
        crc ^= parcel[i];
        for (int j = 0; j < 8; j++)
            (crc & 0x0001) ? crc = (crc >> 1) ^ 0xA001 : crc >>= 1;
    }
    return crc;
}
int KDS::getDev(int addr)
{
    if (CpOpen2(EV_RXFLAG, '\r') > 0) {
        vector<char> parcel;
        char ch[100];
        sprintf(ch, ":%d;0;", addr);
        CpWrite(ch, sprintf(ch, "%s%d\r", ch, CalcCrc(ch)));
        if (CpReadWait(parcel, 1000) > 0)
            dev = getUintData(parcel);
        else
            dev = 0;
        CpClose();
        return dev;
    }
    else
        return 0;
}

int KDS::getSerNum() const
{
    return serNum;
}

void KDS::setSerNum(int value)
{
    serNum = value;
}

int KDS::getAddress() const
{
    return address;
}
void KDS::setAddress(int value)
{
    address = value;
}
int KDS::getChMum() const
{
    return chMum;
}

void KDS::setChMum(int value)
{
    chMum = value;
}
int KDS::getBaudRate() const
{
    return baudRate;
}

void KDS::setBaudRate(int value)
{
    baudRate = value;
}
int KDS::getOutPin() const
{
    return outPin;
}

void KDS::setOutPin(int value)
{
    outPin = value;
}
bool KDS::setOut(int addr, int value)
{
    bool fl;

    if (CpOpen2(EV_RXFLAG, '\r') > 0) {
        vector<char> parcel;
        char ch[100];
        sprintf(ch, ":%d;4;%d;", addr, value);
        CpWrite(ch, sprintf(ch, "%s%d\r", ch, CalcCrc(ch)));
        if (CpReadWait(parcel, 1000) > 0)
            fl = getSuccess(parcel);
        else
            fl = false;
        CpClose();
        return fl;
    }
    else
        return false;
}

uint KDS::getUintData(vector<char> data, bool* ok)
{
    QByteArray ar = QByteArray::fromRawData(data.data(), data.size());

    qDebug() << ar;
    int i = 0;
    while (ar[0] != '!' && ar.size()) {
        ar = ar.remove(0, 1);
    }
    while (ar[ar.size() - 1] != '\r' && ar.size()) {
        ar = ar.remove(ar.size() - 1, 1);
    }
    ar = ar.remove(ar.size() - 1, 1);

    QList<QByteArray> list = ar.split(';');
    ar.clear();

    while (i < list.count() - 1) {
        ar.append(list[i++]).append(';');
    }
    ar.append('\0');

    if (CalcCrc(ar.data()) == list.last().toInt() && list.count() > 2) {
        if (ok)
            *ok = true;
        return list.at(1).toUInt();
    }
    else if (ok)
        *ok = false;
    return 0;
}

bool KDS::getSuccess(vector<char> data)
{
    QByteArray ar = QByteArray::fromRawData(data.data(), data.size());

    qDebug() << ar;
    int i = 0;
    while (ar[0] != '!' && ar.size()) {
        ar = ar.remove(0, 1);
    }
    while (ar[ar.size() - 1] != '\r' && ar.size()) {
        ar = ar.remove(ar.size() - 1, 1);
    }
    ar = ar.remove(ar.size() - 1, 1);

    QList<QByteArray> list = ar.split(';');
    ar.clear();

    while (i < list.count() - 1) {
        ar.append(list[i++]).append(';');
    }
    ar.append('\0');

    if (CalcCrc(ar.data()) == list.last().toInt() && list.count() > 2) {
        if (list.at(1) == "$0")
            return true;
        else
            return false;
    }
    else
        return false;
}
