#ifndef SPEKTR_H
#define SPEKTR_H
#include <QThread>
#include <iostream>
#include <fstream>
#include <cmath>
#include <QString>
#include <QTimer>
#include "usbcom.h"

using namespace std;

class Calculate : public QThread
{
    Q_OBJECT
public:
    Calculate();
    void SetParametrs(int BX, int BY, int EX, int EY, unsigned int SR);
    QString NameFile;
    unsigned int SamplingRate; // частота дискретизации
    int BeginX, BeginY;
    int EndX, EndY;
    unsigned int CountSamplingShow;
    bool TimerWork=false;
    uint16_t* InBuf;
    double* InBufForSpectr;
    unsigned int SizeInBuf;

    void CalculateDFT();
    //void CalculateTimeGraf();
    void CalculateFFT(); //uint16_t* IN, unsigned int* Out, int Count
    uint8_t TypeFunc;
private:
    int DFT(uint16_t *buf, uint16_t *OutBuf, unsigned int n);
    int Frequency(uint16_t value);
    double Period(uint16_t value);
    int CalculateFrequency();
signals:
       //void OutResult(unsigned int secund, unsigned int* data, unsigned int* dataX, unsigned int SizeArray);
      // void OutDataTimeGraf(unsigned int* Data, unsigned int SizeArray);
};
//------------------------------------------------------------------------------
class CalculateTimeGrafic : public Calculate
{
    Q_OBJECT
public:
    CalculateTimeGrafic(int BX, int BY, int EX, int EY, unsigned int SR);
    void run();
private:
    QTimer* tmr;
signals:
       void OutDataTimeGraf(unsigned int* Data, unsigned int SizeArray);
};
//---------------------------------------------------------------------------------
class CalculateFFT : public Calculate
{
    Q_OBJECT
public:
    CalculateFFT (int BX, int BY, int EX, int EY, unsigned int SR);
    void run();
private:
    const double TwoPi = 6.283185307179586;
signals:
       void OutResult(unsigned int secund, unsigned int* data, unsigned int* dataX, unsigned int SizeArray);
};

#endif // SPEKTR_H
