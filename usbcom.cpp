#include "usbcom.h"

UsbCom::UsbCom()
{

}

void UsbCom::run()
{
   OneReadDate = 2000;
   //SizeBuferPort = 2048;
    //setup COM port
   serial.setPortName("COM8"); //NamePort
   serial.setBaudRate(QSerialPort::Baud115200);
   serial.setDataBits(QSerialPort::Data8);
   serial.setParity(QSerialPort::NoParity);
   serial.setStopBits(QSerialPort::OneStop);
   serial.setFlowControl(QSerialPort::NoFlowControl);
   serial.open(QSerialPort::ReadWrite);
   if (serial.isOpen())
   {
    work=true;
    NameFile = "data"+QDateTime::currentDateTime().toString("dd_HH_mm_ss")+".raw";
    File.setFileName(NameFile);
    File.open(QIODevice::WriteOnly);
    qint64 maxSize;
    qint64 readSize;
   // char data[SizeBuferPort];
    Savelog("Начинаем чтение");
    uint16_t Cursor=0;
    while(work)
    {
         serial.waitForReadyRead(1);
         maxSize = serial.bytesAvailable();
         if (maxSize >= OneReadDate)
         {
            readSize = serial.read(&PortBuf[Cursor], OneReadDate);
            emit OutData(Cursor);
            File.write(&PortBuf[Cursor], readSize);
            File.flush();
            Savelog("Готово к чтению: "+QString::number(maxSize)+ " Прочитано:"+QString::number(readSize));
            Cursor+=readSize;
            if (Cursor>=sizeof(PortBuf)) Cursor = 0;
         }
    }
    serial.close();
    File.close();
    Savelog("RUN выполнен");
    }
   else Savelog("Порт не открыт");
}

void UsbCom::Savelog(QString S) {
    std::ofstream out;                       // поток для записи
    out.open("LogFile.txt", std::ios::app); // окрываем файл для записи
    if (out.is_open())
    {
        QString d = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
        out << d.toStdString()+"   "+S.toStdString() << std::endl;
     }
}
/*
void UsbCom::OutData()
{

}

*/
