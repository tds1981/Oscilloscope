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
    int  CalculateFrequency(uint16_t value);
    double  CalculatePeriod(uint16_t value);
   // WAVHEADER WavHeader(uint32_t  sampleRate);
public:
       bool DoDataForSpectr = false;
       bool CalculFrequency = false;
       char TypeFile[5]="wav";
       int Frequency = -1; //вычисленная частота сигнала
       double Period = -1;
       bool work=false;
       QString NamePort;
       QString NameFile;
       QSerialPort serial;
       QFile File;
signals:
       void OutData(uint16_t* buf, unsigned int SizeData);
       void OutDataSpectr(double* buf, unsigned int SizeData);

};

#endif // USBCOM_H
