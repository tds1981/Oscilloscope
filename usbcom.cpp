#include "usbcom.h"

UsbCom::UsbCom()
{

}

void UsbCom::run()
{
   uint32_t OneReadDate = 2*SamplingRate/100;
    //setup COM port
   serial.setPortName(NamePort); //NamePort
   serial.setBaudRate(1024000);
   serial.setDataBits(QSerialPort::Data8);
   serial.setParity(QSerialPort::NoParity);
   serial.setStopBits(QSerialPort::OneStop);
   serial.setFlowControl(QSerialPort::NoFlowControl);
   serial.open(QSerialPort::ReadWrite);
   if (serial.isOpen())
   {
       serial.clear();
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
    uint32_t Cursor=0;
    uint16_t* Port = new uint16_t[SamplingRate];
    while(work)
    {
         serial.waitForReadyRead(1);
         maxSize = serial.bytesAvailable();
         if (maxSize >= OneReadDate)
         { 
            if((maxSize<(SamplingRate-Cursor)*2)&&(maxSize%2==0)) OneReadDate = maxSize;
            else OneReadDate = (SamplingRate-Cursor)*2;
            char* p = reinterpret_cast<char*>(&Port[Cursor]);
            readSize = serial.read(p, OneReadDate);
            for(uint32_t k=0; k<readSize/2; k++) Port[Cursor+k] = Port[Cursor+k]>>1;
            File.write(p, readSize);
            File.flush();
            Savelog("Было Готово к чтению: "+QString::number(maxSize)+ " Прочитано:"+QString::number(readSize));
            Cursor+=readSize/2;
            if (Cursor>=SamplingRate)
            {            
                emit OutData(Port);
                Cursor = 0;
                Port = new uint16_t[SamplingRate];
            }
         }
    }
    serial.close();
    if (TypeFile[0] == 'w')
    {
        Savelog("Перезапись WAV Заголовка");
        File.seek(0);
        WAVHEADER WH=WavHeader(SamplingRate, File.size());
        File.write(reinterpret_cast<char*>(&WH), sizeof(WH));
        File.flush();
    }
    File.close();
    delete [] Port;
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

