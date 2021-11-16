#include "usbcom.h"

UsbCom::UsbCom()
{

}

int UsbCom::CalculateFrequency(uint16_t value)
{
    static int rise=0, max=0, min=0, fall=0, period=0; //,
    static uint16_t v0;

    // вычисляем количество переходов через максимальное значение
    if (period == 0) {max=0; min=0;}
    if (value>v0) {rise++;}
    if (value<v0) {fall++;}
    if ((value<v0)&&(rise>3)) {rise=0; max++;}
    if ((value>v0)&&(fall>3)) {fall=0; min++;}

    v0 = value;
    period++;
    if (period == SamplingRate) {period=0; return (max+min)/2;} //
    else return -1;
}

double UsbCom::CalculatePeriod(uint16_t value)
{
    static int rise=0, period=0; //max=0, min=0, fall=0,
    static uint16_t v0;
    double PeriodReturn = -1;
    // вычисляем количество переходов через максимальное значение
    if (value>v0) {rise++;}
    //if (value<v0) {fall++;}
    if ((value<v0)&&(rise>3)) {rise=0; PeriodReturn = (double)period / SamplingRate; period = 0;}
    //if ((value>v0)&&(fall>3)) {fall=0; }

    v0 = value;
    period++;
    return PeriodReturn;

}

void UsbCom::run()
{
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
    unsigned int SizeDataOut=SamplingRate/10;
    uint32_t OneReadDate = SizeDataOut;
    uint16_t* Port = new uint16_t[SizeDataOut];
    double* DataForSpectr = nullptr;
    unsigned int CursorDataForSpectr=0;
    while(work)
    {
         serial.waitForReadyRead(1);
         maxSize = serial.bytesAvailable();
         if (maxSize >= OneReadDate)
         { 
            if((maxSize<(SizeDataOut-Cursor)*2)&&(maxSize%2==0)) OneReadDate = (uint32_t)maxSize;
            else OneReadDate = (SizeDataOut-Cursor)*2;
            char* p = reinterpret_cast<char*>(&Port[Cursor]);
            readSize = serial.read(p, OneReadDate);
            for(uint32_t k=0; k<readSize/2; k++) Port[Cursor+k] = Port[Cursor+k]>>1;
            File.write(p, readSize);
            File.flush();
            Savelog("Было Готово к чтению: "+QString::number(maxSize)+ " Прочитано:"+QString::number(readSize));
            Cursor+=readSize/2;
            if (Cursor==SizeDataOut)
            {
                if (CalculFrequency)
                {
                    if ((DataForSpectr==nullptr)&&(DoDataForSpectr)) DataForSpectr = new double[SamplingRate+31072];
                    for(unsigned int j=0; j<SizeDataOut; j++)
                    {
                        if (DoDataForSpectr) DataForSpectr[CursorDataForSpectr++]=static_cast<double>(Port[j]);
                        int f = CalculateFrequency(Port[j]);
                        double  periodBuf = CalculatePeriod(Port[j]);
                        if (f != -1) Frequency = f;
                        if (periodBuf != -1) Period = periodBuf;
                    }
                    if ((CursorDataForSpectr==SamplingRate)&&(DoDataForSpectr))
                    {
                        emit OutDataSpectr(DataForSpectr, SamplingRate+31072);
                        CursorDataForSpectr=0;
                        DataForSpectr=nullptr;
                    }
                }

                emit OutData(Port, SizeDataOut);
                Cursor = 0;
                Port = new uint16_t[SizeDataOut];
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

