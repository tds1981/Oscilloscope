#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QDateTime>

#include <iostream>
#include <fstream>
#include <cmath>
#include "wav.h"

using namespace std;

int main(int argc, char *argv[])
{
    unsigned int SamplingRate; //частота дискретизации

    QCoreApplication a(argc, argv);
    char NamePort[10];
    cout << "Input name port:" << endl;
    cin >> NamePort;
    cin.get();

    cout << "How much seconds will be record?:"<<endl;
    unsigned int time;
    cin >> time;
    cin.get();

    cout << "SamplingRate?:"<<endl;
    cin >> SamplingRate;
    cin.get();

    QSerialPort serial;
    QFile File;

    serial.setPortName(NamePort); //NamePort
    serial.setBaudRate(2048000);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QSerialPort::ReadWrite);
    if (serial.isOpen())
    {
     serial.clear();
   //  work=true;
   //  NameFile = "data"+QDateTime::currentDateTime().toString("dd_HH_mm_ss")+"."+TypeFile; //"raw";
     File.setFileName("data"+QDateTime::currentDateTime().toString("dd_HH_mm_ss")+".wav");
     File.open(QIODevice::WriteOnly);

     char b[44]={0}; // резервируем место под wav заголовок
     File.write(b, sizeof(b));
     File.flush();
    }
    qint64 maxSize;
    qint64 readSize;
    qint64 timeRecord=0;
    while(time > timeRecord/SamplingRate)
    {
             serial.waitForReadyRead(1);
             maxSize = serial.bytesAvailable();
             if (maxSize >= SamplingRate/2)
             {
                char* Buf = new char[maxSize];
                uint16_t* p = reinterpret_cast<uint16_t*>(Buf);
                readSize = serial.read(Buf, maxSize);
                for(uint32_t k=0; k<readSize/2; k++) p[k] = p[k]>>1;
                File.write(Buf, readSize);
                File.flush();
                timeRecord+=readSize/2;
                 cout << "Time: "<<timeRecord/SamplingRate<<" Size file: "<<timeRecord*2<<endl;
                delete [] Buf;
             }
     }
    serial.close();
    cout << "Wav record"<<endl;
    File.seek(0);
    WAVHEADER WH=WavHeader(SamplingRate, File.size());
    File.write(reinterpret_cast<char*>(&WH), sizeof(WH));
    File.flush();
    File.close();
    cin.get();
    return a.exec();
}
