#include "usbcom.h"

//------------- class FileSave ------------------------------------------------------
FileSave::FileSave()
{

}

void FileSave::run()
{
    while(!deq.empty())
    {
        Array Buf = deq.back();

        uint16_t* p = reinterpret_cast<uint16_t*>(Buf.data);
        for(uint32_t k=0; k<Buf.size/2; k++) p[k] = p[k]>>1;

        if (out.is_open()) out.write(Buf.data, Buf.size);
        emit OutData(p, Buf.size/2);
        SizeFile+=Buf.size;
        Savelog("Сохранено в файл: "+QString::number(Buf.size)+" Размер файла: "+QString::number(SizeFile)+" Очередь: "+QString::number(deq.size()));
        deq.pop_back();
    }
}

void FileSave::OpenFile()
{
    NameFile = "data"+QDateTime::currentDateTime().toString("dd_HH_mm_ss")+"."+TypeFile; //"raw";                       // поток для записи
    out.open(NameFile.toUtf8(), std::ios::binary); // окрываем файл для записи
    if (TypeFile[0] == 'w')
    {
        //out.seekp(sizeof(WAVHEADER));
        WAVHEADER WH=WavHeader(SamplingRate, 44);
        out.write(reinterpret_cast<char*>(&WH), sizeof(WH));
    }
}

void FileSave::SaveWav()
{
    if (TypeFile[0] == 'w')
    {
        Savelog("Перезапись WAV Заголовка");
        WAVHEADER WH=WavHeader(SamplingRate, SizeFile+44);
        out.seekp(0);
        out.write(reinterpret_cast<char*>(&WH), sizeof(WH));
    }
     out.close();
}

void FileSave::Savelog(QString S) {
    std::ofstream out;                       // поток для записи
    out.open("LogFile.txt", std::ios::app); // окрываем файл для записи
    if (out.is_open())
    {
        QString d = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
        out << d.toStdString()+"   "+S.toStdString() << std::endl;
     }
}
//------------------------ class UsbCom  -------------------------------------------------------

UsbCom::UsbCom()
{
    serial = new QSerialPort;
    File = new FileSave();
}

bool UsbCom::OpenSerial()
{
    File->OpenFile();
    //setup COM port
   serial->setPortName(NamePort); //NamePort
   serial->setBaudRate(1024000);
   serial->setDataBits(QSerialPort::Data8);
   serial->setParity(QSerialPort::NoParity);
   serial->setStopBits(QSerialPort::OneStop);
   serial->setFlowControl(QSerialPort::NoFlowControl);
   serial->open(QSerialPort::ReadOnly); //ReadWrite
   if (serial->isOpen()) File->Savelog("Порт открыт. Начинаем чтение");
   serial->clear();
   return serial->isOpen();
}

bool UsbCom::CloseSerial()
{
    while(this->isRunning());
    serial->close();
    File->SaveWav();
    File->Savelog("Порт закрыт");
    return !serial->isOpen();
}

void UsbCom::run()
{
    qint64 maxSize;
    qint64 readSize;
    Array Buf;
    while (!serial->atEnd())
    {
        maxSize = serial->bytesAvailable();
        Buf.size = static_cast<unsigned int>(maxSize);
        Buf.data = new char[Buf.size];
        readSize = serial->read(Buf.data, maxSize);
        //Buf.size = static_cast<unsigned int>(readSize);
        File->deq.push_front(Buf);
    }
    if (!File->isRunning()) File->start(QThread::LowestPriority);
}



