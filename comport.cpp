#include "comport.h"

COMPORT::COMPORT()
    : MyEvFunc(0)
    , MyRdFunc(0)
    , hRdThr(0)
    , hWrThr(0)
    , hComport(0)
    , dwWrFlag(0)
    , portNum(0)
    , baudRate(9600)
{
    RdThread.Method = &COMPORT::ReadThread; //it's magic
    WrThread.Method = &COMPORT::WriteThread; //it's magic
    QStringList List;
    CpEnum(List);
}

COMPORT::~COMPORT()
{
    CpClose();
}

long COMPORT::CpEnum(QStringList& List)
{
    DWORD ComportCounter;
    GUID GUID_CLASS_COMPORT = { 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 };

    vector<PCHAR>::iterator it = Addres.begin();
    while (it != Addres.end())
        delete *it++;
    Addres.clear();

    HDEVINFO hDeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
    if (hDeviceInfoSet != INVALID_HANDLE_VALUE) {
        HDEVINFO hNewDeviceInfoSet = SetupDiGetClassDevsExW(&GUID_CLASS_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, hDeviceInfoSet, NULL, NULL);
        if (hNewDeviceInfoSet != INVALID_HANDLE_VALUE) {
            SP_DEVICE_INTERFACE_DATA itf = { sizeof(SP_DEVICE_INTERFACE_DATA) };
            //Сколько портов есть в системе
            for (ComportCounter = 0; ::SetupDiEnumDeviceInterfaces(hNewDeviceInfoSet, NULL, &GUID_CLASS_COMPORT, ComportCounter, &itf); ++ComportCounter)
                continue;
            if (ComportCounter == 0)
                return -1;

            Addres.resize(ComportCounter);
            for (ComportCounter = 0; ::SetupDiEnumDeviceInterfaces(hNewDeviceInfoSet, NULL, &GUID_CLASS_COMPORT, ComportCounter, &itf); ++ComportCounter) {
                DWORD dwSize = 0;
                SP_DEVINFO_DATA dev = { sizeof(SP_DEVINFO_DATA) };
                //Получить путь, длина строки.
                vector<BYTE> buffer;
                ::SetupDiGetDeviceInterfaceDetailA(hNewDeviceInfoSet, &itf, NULL, 0, &dwSize, &dev);
                if (dwSize > 0) {
                    buffer.resize(dwSize);
                    SP_DEVICE_INTERFACE_DETAIL_DATA_A* pDetail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_A*>(buffer.data());
                    pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
                    ::SetupDiGetDeviceInterfaceDetailA(hNewDeviceInfoSet, &itf, pDetail, dwSize, NULL, &dev);
                    //					pShortName[ComportCounter] = SysAllocStringByteLen(pDetail->DevicePath, dwSize - 5);
                    PCHAR ch = new CHAR[dwSize - 4];
                    memset(ch, 0, dwSize - 4);
                    memcpy(ch, pDetail->DevicePath, dwSize - 5);
                    Addres.at(ComportCounter) = ch;
                }
                //Получить человеческое название, длина строки.
                DWORD dwType = 0;
                ::SetupDiGetDeviceRegistryPropertyA(hNewDeviceInfoSet, &dev, SPDRP_FRIENDLYNAME, &dwType, NULL, 0, &dwSize);
                if (dwSize > 0) {
                    buffer.resize(dwSize);
                    ::SetupDiGetDeviceRegistryPropertyA(hNewDeviceInfoSet, &dev, SPDRP_FRIENDLYNAME, NULL, buffer.data(), dwSize, NULL);
                    QString str((char*)buffer.data());
                    str = str.remove(0, str.indexOf('(') + 1);
                    str = str.remove(str.size() - 1, 1);
                    List << str; //QString((char*)buffer.data());
                }
            }
        }
        SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
        return ComportCounter;
    }
    else
        return -1;
}

long COMPORT::CpOpen2(DWORD EvtFlag, long EvtChar)
{
    HANDLE tmp;
    return CpOpen(&tmp, baudRate, EvtFlag, EvtChar, Addres.at(portNum));
}

long COMPORT::CpOpen(long PortName, DWORD Speed, DWORD EvtFlag, long EvtChar)
{
    HANDLE tmp;
    return CpOpen(&tmp, Speed, EvtFlag, EvtChar, Addres.at(PortName));
}

long COMPORT::CpOpen(HANDLE* Handle, DWORD Speed, DWORD EvtFlag, long EvtChar, LPSTR PortName)
{
    dwEvRdFlag = EvtFlag;
    DCB dcb;
    COMMTIMEOUTS timeouts;
    if (!hComport) {
        hComport = ::CreateFileA(PortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        //если ошибка открытия порта
        qDebug() << "hComport =" << hComport;
        if (hComport == INVALID_HANDLE_VALUE) {
            return -1;
        }
        dcb.DCBlength = sizeof(DCB);
        if (!::GetCommState(hComport, &dcb)) {
            CpClose();
            return -2;
        }
        dcb.BaudRate = Speed; //задаём скорость передачи X бод
        dcb.fBinary = TRUE; //включаем двоичный режим обмена
        dcb.fOutxCtsFlow = FALSE; //выключаем режим слежения за сигналом CTS
        dcb.fOutxDsrFlow = FALSE; //выключаем режим слежения за сигналом DSR
        dcb.fDtrControl = DTR_CONTROL_DISABLE; //отключаем использование линии DTR
        dcb.fDsrSensitivity = FALSE; //отключаем восприимчивость драйвера к состоянию линии DSR
        dcb.fNull = FALSE; //разрешить приём нулевых байтов
        dcb.fRtsControl = RTS_CONTROL_DISABLE; //отключаем использование линии RTS
        dcb.fAbortOnError = FALSE; //отключаем остановку всех операций чтения/записи при ошибке
        dcb.ByteSize = 8; //задаём 8 бит в байте
        dcb.Parity = 0; //отключаем проверку чётности
        dcb.StopBits = 0;
        dcb.EvtChar = (char)EvtChar;
        //загрузить структуру DCB в порт
        //если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
        if (!::SetCommState(hComport, &dcb)) {
            CpClose();
            return -3;
        }
        //установить таймауты
        timeouts.ReadIntervalTimeout = 10; //таймаут между двумя символами
        timeouts.ReadTotalTimeoutMultiplier = 1; //общий таймаут операции чтения
        timeouts.ReadTotalTimeoutConstant = 100; //константа для общего таймаута операции чтения
        timeouts.WriteTotalTimeoutMultiplier = 0; //общий таймаут операции записи
        timeouts.WriteTotalTimeoutConstant = 0; //константа для общего таймаута операции записи
        //записать структуру таймаутов в порт
        //если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
        if (!::SetCommTimeouts(hComport, &timeouts)) {
            CpClose();
            return -4;
        }
        ::SetupComm(hComport, 0xFFF, 0xFFF); //установить размеры очередей приёма и передачи
        ::PurgeComm(hComport, PURGE_RXCLEAR); //очистить принимающий буфер порта

        hRdThr = ::CreateThread(NULL, 500, RdThread.Function, this, 0, NULL); //создаём поток чтения, который сразу начнёт выполняться (предпоследний параметр = 0)
        hWrThr = ::CreateThread(NULL, 500, WrThread.Function, this, CREATE_SUSPENDED, NULL); //создаём поток записи в остановленном состоянии (предпоследний параметр = CREATE_SUSPENDED)
        if (!hRdThr || !hWrThr) {
            CpClose();
            return -5;
        }
        *Handle = hComport;
        /*hMutexWr = CreateMutex(
            NULL,              // default security attributes
            FALSE,             // initially not owned
            NULL);             // unnamed mutex
        hMutexRd = CreateMutex(
            NULL,              // default security attributes
            FALSE,             // initially not owned
            NULL);             // unnamed mutex*/
    }
    return 1;
}

long COMPORT::CpClose()
{
    long FLAG;
    if (hComport > 0) {
        if (hWrThr > 0) //если поток записи работает, завершить его; проверка if обязательна, иначе возникают ошибки
        {
            ::TerminateThread(hWrThr, NULL);
            ::CloseHandle(OlWr.hEvent); //нужно закрыть объект-событие
            //WaitForSingleObject(WR_THR._HANDLE, INFINITE);
            ::CloseHandle(hWrThr);
            hWrThr = 0;
        }
        if (hRdThr > 0) //если поток чтения работает, завершить его; проверка if обязательна, иначе возникают ошибки
        {
            ::TerminateThread(hRdThr, NULL);
            ::CloseHandle(OlRd.hEvent); //нужно закрыть объект-событие
            //WaitForSingleObject(RD_THR._HANDLE, INFINITE);
            ::CloseHandle(hRdThr);
            hRdThr = 0;
        }
        FLAG = ::CloseHandle(hComport); //закрыть порт
        hComport = 0; //обнулить переменную дескриптора порта
    }
    return FLAG;
}

long COMPORT::CpWrite(vector<char> Data)
{
    if (Data.size() == 0)
        return -1;
    BufWr.resize(Data.size()); //Изменение размера буфера передачи
    memcpy(BufWr.data(), Data.data(), Data.size()); //копированние данных в буфер
    ::ResumeThread(hWrThr); //Запуск записи
    return BufWr.size(); //Возврат количества записанных байт
}

long COMPORT::CpWrite(char* pchar, int len)
{
    vector<char> data;
    if (len > 0) {
        data.resize(len);
        memcpy(data.data(), pchar, len);
        return CpWrite(data);
    }
    else {
        data.clear();
        int i = 0;
        while (pchar[i] != 0 && i < 0xFFF) {
            data.push_back(pchar[i++]);
        }
        return CpWrite(data);
    }
}

long COMPORT::CpRead(vector<char>& Data)
{
    long len;
    //Есть что читать
    if (BufRd.size() > 0) {
        Data.resize(BufRd.size());
        ::memcpy(Data.data(), BufRd.data(), BufRd.size());
        len = BufRd.size();
        //"Освобождение"
        BufRd.clear();
        return len;
    }
    else
        return -1;
}

long COMPORT::CpReadWait(vector<char>& Data, int time)
{
    dataFl = time;
    while (0 < dataFl--) {
        Sleep(1);
    }
    return CpRead(Data);
    //    rdMutex.lock();
    //    if (rdMutex.tryLock(time)) {
    //        rdMutex.unlock();
    //        return CpRead(Data);
    //    }
    //    else {
    //        rdMutex.unlock();
    //        Data.clear();
    //        return -1;
    //    }
}

long COMPORT::CpRTS(bool FLAG)
{
    if (FLAG)
        return ::EscapeCommFunction(hComport, SETRTS);
    else
        return ::EscapeCommFunction(hComport, CLRRTS);
}

long COMPORT::CpDTR(bool FLAG)
{
    if (FLAG)
        return ::EscapeCommFunction(hComport, SETDTR);
    else
        return ::EscapeCommFunction(hComport, CLRDTR);
}

long COMPORT::CpReadFunc(pRdFuncType AddrFunc)
{
    MyRdFunc = AddrFunc;
    return long(MyRdFunc);
}

long COMPORT::CpEventFunc(pEvFuncType AddrFunc)
{
    MyEvFunc = AddrFunc;
    return long(MyEvFunc);
}
int COMPORT::CpGetPortNum() const
{
    return portNum;
}

void COMPORT::CpSetPortNum(int value)
{

    portNum = value;
}

DWORD COMPORT::CpGetBaudRate() const
{
    return baudRate;
}

void COMPORT::CpSetBaudRate(DWORD value)
{
    baudRate = value;
}

void COMPORT::CpSetBaudRate(QString value)
{
    baudRate = value.toUInt();
}

DWORD COMPORT::WriteThread(LPVOID)
{
    //temp - переменная-заглушка
    DWORD temp, signal;
    //создать событие
    OlWr.hEvent = ::CreateEventA(NULL, true, true, NULL);
    while (1) {
        //записать байты в порт (перекрываемая операция!)
        ::WriteFile(hComport, BufWr.data(), BufWr.size(), &temp, &OlWr);
        //приостановить поток, пока не завершится перекрываемая операция WriteFile
        signal = ::WaitForSingleObject(OlWr.hEvent, INFINITE);
        //если операция завершилась успешно
        if ((signal == WAIT_OBJECT_0) && (::GetOverlappedResult(hComport, &OlWr, &temp, true)))
            dwWrFlag = 1;
        else
            dwWrFlag = -1;
        ::SuspendThread(hWrThr);
    }
    return 0;
}

DWORD COMPORT::ReadThread(LPVOID)
{
    //BYTE* pData = NULL;
    //структура текущего состояния порта
    //в данной программе используется для определения количества принятых в порт байтов
    COMSTAT comstat;
    //переменная temp используется в качестве заглушки
    DWORD dwMask, dwSignal, PinStatus, temp, dwByteRead;
    //создать сигнальный объект-событие для асинхронных операций
    OlRd.hEvent = ::CreateEventA(NULL, true, true, NULL);
    //установить маску на срабатывание по событию приёма байта в порт
    ::SetCommMask(hComport, dwEvRdFlag);
    //пока поток не будет прерван, выполняем цикл
    while (1) {
        dwByteRead = 0;
        //ожидать события приёма байта (это и есть перекрываемая операция)
        ::WaitCommEvent(hComport, &dwMask, &OlRd);
        //приостановить поток до прихода байта
        dwSignal = ::WaitForSingleObject(OlRd.hEvent, INFINITE);
        //если событие прихода байта произошло
        if (dwSignal == WAIT_OBJECT_0) {
            //проверяем, успешно ли завершилась перекрываемая операция WaitCommEvent
            if (::GetOverlappedResult(hComport, &OlRd, &temp, true)) {
                //если произошло именно событие прихода байта
                if ((dwMask & dwEvRdFlag) != 0) {
                    //Отслеживание других линий

                    if (MyEvFunc != 0) {
                        ::GetCommModemStatus(hComport, &PinStatus);
                        (*MyEvFunc)(dwMask, PinStatus); //Вызвать каллбак события
                    }
                    //нужно заполнить структуру COMSTAT
                    ::ClearCommError(hComport, &temp, &comstat);
                    //и получить из неё количество принятых байтов
                    dwByteRead = comstat.cbInQue;
                    //если действительно есть байты для чтения
                    if (dwByteRead) {
                        BufRd.resize(BufRd.size() + dwByteRead, 0);
                        char* data = BufRd.data() + (BufRd.size() - dwByteRead);
                        //прочитать байты из порта в буфер программы
                        ::ReadFile(hComport, data, dwByteRead, &temp, &OlRd);
                        //Если есть фунция "калбак"
                        //rdMutex.unlock();
                        dataFl = 0;
                        emit ReadEvent(BufRd);
                        if (MyRdFunc != 0) {
                            //                            if (Data) {
                            //                                SAFEARRAYBOUND saBound;
                            //                                saBound.lLbound = 0;
                            //                                saBound.cElements = BufRd.size();
                            //                                //Переразмер  "куда возвращать"
                            //                                if (SUCCEEDED(::SafeArrayRedim(Data, &saBound))) {
                            //                                    if (SUCCEEDED(::SafeArrayAccessData(Data, (void**)&pData))) {
                            //                                        ::memcpy(pData, BufRd.data(), BufRd.size());
                            //                                        ::SafeArrayUnaccessData(Data);
                            //                                        BufRd.clear();
                            //                                        (*MyRdFunc)(&Data);
                            //                                    }
                            //                                }
                            //                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
