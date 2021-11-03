#ifndef USBCOM_H
#define USBCOM_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
//#include <QByteArray>
#include <QFile>

#include <fstream>
#include <windows.h>
#include <iostream>
#include <QString>
#include <string>
#include <QDateTime>

using namespace std;

#define SizeBuferPort 20000

class UsbCom : public QThread
{
     Q_OBJECT
public:
    UsbCom();
    void run();
    void Savelog(QString S);
public:
       char PortBuf[SizeBuferPort]; //1 секунда, частота оцифровки 10k
       uint16_t OneReadDate;
       bool work=false;
       QString NamePort;
       QString NameFile;
       QSerialPort serial;
       QFile File;
signals:
       void OutData(uint16_t n);

};

#endif // USBCOM_H
