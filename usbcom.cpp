#include "usbcom.h"

UsbCom::UsbCom()
{

}

void UsbCom::run()
{
   OneReadDate = 2000;
   //SizeBuferPort = 2048;
    //setup COM port
   serial.setPortName(NamePort); //NamePort
   serial.setBaudRate(512000);
   serial.setDataBits(QSerialPort::Data8);
   serial.setParity(QSerialPort::NoParity);
   serial.setStopBits(QSerialPort::OneStop);
   serial.setFlowControl(QSerialPort::NoFlowControl);
   serial.open(QSerialPort::ReadWrite);
   if (serial.isOpen())
   {
    work=true;
    NameFile = "data"+QDateTime::currentDateTime().toString("dd_HH_mm_ss")+"."+TypeFile; //"raw";
    File.setFileName(NameFile);
    File.open(QIODevice::WriteOnly);
    if (TypeFile[0] == 'w')
    {
        char b[44]={0}; // резервируем место под wav заголовок
        File.write(b, sizeof(b));
        File.flush();
    }
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
            uint16_t* p = reinterpret_cast<uint16_t*>(&PortBuf[Cursor]);
            for(uint16_t k=0; k<OneReadDate/2; k++) p[k] = p[k]>>1;
            emit OutData(Cursor);
            File.write(&PortBuf[Cursor], readSize);
            File.flush();
            Savelog("Готово к чтению: "+QString::number(maxSize)+ " Прочитано:"+QString::number(readSize));
            Cursor+=readSize;
            if (Cursor>=sizeof(PortBuf)) Cursor = 0;
         }
    }
    serial.close();
    if (TypeFile[0] == 'w')
    {
        Savelog("Перезапись WAV Заголовка");
        File.seek(0);
        WAVHEADER WH=WavHeader(SizeBuferPort/2, File.size());
        File.write(reinterpret_cast<char*>(&WH), sizeof(WH));
        File.flush();
    }
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

