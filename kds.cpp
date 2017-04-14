#include "kds.h"

#include <QDebug>

KDS::KDS()
{
    setBaudRate(Baud9600);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
}

QString KDS::CalcCrc(QByteArray& parcel)
{
    uint crc = 0xFFFF;
    for (int i = 1; i < parcel.size(); i++) {
        crc ^= parcel[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            }
            else {
                crc >>= 1;
            }
        }
    }
    return QString().setNum(crc);
}

//QString KDS::CalcCrc(const char* parcel)
//{
//    uint crc = 0xFFFF;
//    int i = 0;
//    while (parcel[++i] != 0) {
//        crc ^= parcel[i];
//        for (int j = 0; j < 8; j++)
//            (crc & 0x0001) ? crc = (crc >> 1) ^ 0xA001 : crc >>= 1;
//    }
//    return QString().setNum(crc);
//}

int KDS::getDev(int addr)
{
    if (open(ReadWrite)) {
        QByteArray data = QString(":%1;0;").arg(addr).toLocal8Bit();
        data.append(CalcCrc(data)).append('\r');
        write(data);
        waitForReadyRead(100);
        dev = getUintData(readAll());
        close();
        return dev;
    }
    return 0;
}

bool KDS::setOut(int addr, int value)
{
    if (open(ReadWrite)) {
        QByteArray data = QString(":%1;4;%2;").arg(addr).arg(value).toLocal8Bit();
        data.append(CalcCrc(data)).append('\r');
        write(data);
        waitForReadyRead(100);
        bool fl = getSuccess(readAll());
        close();
        return fl;
    }
    return false;
}

uint KDS::getUintData(QByteArray data)
{
    if (data.isEmpty()) {
        return false;
    }
    int i = 0;
    while (data[0] != '!' && data.size()) {
        data = data.remove(0, 1);
    }
    while (data[data.size() - 1] != '\r' && data.size()) {
        data = data.remove(data.size() - 1, 1);
    }
    data = data.remove(data.size() - 1, 1);

    QList<QByteArray> list = data.split(';');
    data.clear();

    while (i < list.count() - 1) {
        data.append(list[i++]).append(';');
    }
    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2) {
        return list.at(1).toUInt();
    }
    return 0;
}

bool KDS::getSuccess(QByteArray data)
{

    if (data.isEmpty()) {
        return false;
    }
    int i = 0;

    while (data[0] != '!' && data.size()) {
        data = data.remove(0, 1);
    }
    while (data[data.size() - 1] != '\r' && data.size()) {
        data = data.remove(data.size() - 1, 1);
    }
    data = data.remove(data.size() - 1, 1);

    QList<QByteArray> list = data.split(';');
    data.clear();

    while (i < list.count() - 1) {
        data.append(list[i++]).append(';');
    }
    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2) {
        if (list.at(1) == "$0") {
            return true;
        }
    }
    return false;
}
