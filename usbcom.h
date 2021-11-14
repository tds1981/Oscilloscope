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

using namespace std;

const unsigned int SamplingRate = 100000; //частота дискретизации

class UsbCom : public QThread
{
     Q_OBJECT
public:
    UsbCom();
    void run();
    void Savelog(QString S);
   // WAVHEADER WavHeader(uint32_t  sampleRate);
public:
       char TypeFile[5]="wav";
      // uint16_t PortBuf[SamplingRate]; //1 секунда, частота оцифровки 100k
       //uint32_t OneReadDate;
       //uint16_t Faza;
       bool work=false;
       QString NamePort;
       QString NameFile;
       QSerialPort serial;
       QFile File;
signals:
       void OutData(uint16_t* buf);
       //void OutDataSpectr(uint16_t* buf);

};

#endif // USBCOM_H
