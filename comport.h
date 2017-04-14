#ifndef COM_PORT_H
#define COM_PORT_H

#include <QObject>
#include <QDebug>
#include <QStringList>

#include <Windows.h>
#include <SetupAPI.h>
#pragma comment(lib, "setupapi.lib")

#include <vector>

using namespace std;

typedef void(__stdcall* pRdFuncType)(char*);
typedef void(__stdcall* pEvFuncType)(const long, const long);

class COMPORT : public QObject {
    Q_OBJECT
public:
    COMPORT();
    ~COMPORT();

    long CpEnum(QStringList& List);
    long CpOpen2(DWORD EvtFlag = EV_RXCHAR, long EvtChar = 0);
    long CpOpen(long PortName, DWORD Speed = 9600, DWORD EvtFlag = EV_RXCHAR, long EvtChar = 0);
    long CpOpen(HANDLE* Handle, DWORD Speed, DWORD EvtFlag, long EvtChar, LPSTR PortName);
    long CpClose();
    long CpWrite(vector<char> Data);
    long CpWrite(char* pchar, int len = 0);
    long CpRead(vector<char>& Data);
    long CpReadWait(vector<char>& Data, int time = 1000);
    long CpRTS(bool FLAG);
    long CpDTR(bool FLAG);
    long CpReadFunc(pRdFuncType AddrFunc);
    long CpEventFunc(pEvFuncType AddrFunc);

    int CpGetPortNum() const;
    DWORD CpGetBaudRate() const;

public slots:
    void CpSetPortNum(int value);
    void CpSetBaudRate(DWORD value);
    void CpSetBaudRate(QString value);

signals:
    void ReadEvent(vector<char> Data);

private:
    vector<PCHAR> Addres;
    long dataFl;

    int portNum;
    DWORD baudRate;

    QMutex rdMutex;

    pRdFuncType MyRdFunc; //Callback'i
    pEvFuncType MyEvFunc;

    DWORD dwEvRdFlag;
    HANDLE hComport;

    DWORD dwWrFlag; //флаг, указывающий на успешность операций записи (1 - успешно, 0 - не успешно)

    vector<char> BufWr; //Передающий буфер
    vector<char> BufRd; //Принимающий буфер

    //    HANDLE hMutexWr;
    //    HANDLE hMutexRd;

    // объявляем необходимые типы
    // прототип функции потока
    typedef DWORD(WINAPI* ThrdFunc)(LPVOID);
    // прототип метода класса
    typedef DWORD (WINAPI COMPORT::*ClassMethod)(LPVOID);
    // данное объединение позволяет решить несостыковку с типами
    typedef union {
        ThrdFunc Function;
        ClassMethod Method;
    } tThrdAddr;

    tThrdAddr RdThread;
    tThrdAddr WrThread;

    HANDLE hRdThr;
    HANDLE hWrThr;

    OVERLAPPED OlRd; //будем использовать для операций чтения (см. поток ReadThread)
    OVERLAPPED OlWr; //будем использовать для операций записи (см. поток WriteThread)

    virtual DWORD WINAPI WriteThread(LPVOID);
    virtual DWORD WINAPI ReadThread(LPVOID);
};

#endif // COM_PORT_H
