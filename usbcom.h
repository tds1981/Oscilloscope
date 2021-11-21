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
#include "wav.h"
//#include <vector>
#include <deque>

using namespace std;

const unsigned int SamplingRate = 100000; //частота дискретизации

typedef struct
{
    unsigned int  size;
    char* data;
}Array;

class FileSave : public QThread
{
     Q_OBJECT
public:
    FileSave();
    void run();
    void Savelog(QString S);
    void SaveWav();
    void OpenFile();
public:
       char TypeFile[5]="wav";
       QString NameFile;
       uint32_t SizeFile = 0;
       std::ofstream out;  // поток для записи
       std::deque<QByteArray> deq;
signals:
       void OutData(uint16_t* buf, unsigned int SizeData);
};
//-------------------------------------------------------------------------

class UsbCom : public QThread
{
     Q_OBJECT
public:
    UsbCom();
    void run();
    bool OpenSerial();
    bool CloseSerial();
    QString NamePort;
    QSerialPort* serial;
    FileSave *File;
};

#endif // USBCOM_H
