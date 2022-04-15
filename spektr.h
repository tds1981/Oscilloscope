#ifndef SPEKTR_H
#define SPEKTR_H
#include <QThread>
#include <iostream>
#include <fstream>
#include <cmath>
#include <QString>
#include <QTimer>
#include "usbcom.h"
#include <deque>

using namespace std;

typedef struct
{
    bool Delete = false;
    unsigned int  sizeData;
    uint16_t* data;
    double* dataDouble;
}ArraySemples;

class Calculate : public QThread
{
    Q_OBJECT
public:
    Calculate();
    void SetParametrs(int BX, int BY, int EX, int EY, unsigned int SR);
    ArraySemples CreateInBuf(bool Del_accept);
    bool CheckCountElementsInDEQ();
    std::deque<ArraySemples> deqSamples;
    QString NameFile;
    unsigned int SamplingRate; // частота дискретизации
    int BeginX, BeginY;
    int EndX, EndY;
    unsigned int CountSamplingShow;
    //bool TimerWork=false;

    //double* InBufForSpectr;

    void CalculateDFT();
    //void CalculateTimeGraf();
    void CalculateFFT(); //uint16_t* IN, unsigned int* Out, int Count
    uint8_t TypeFunc;
private:

    int DFT(uint16_t *buf, uint16_t *OutBuf, unsigned int n);

signals:
       //void OutResult(unsigned int secund, unsigned int* data, unsigned int* dataX, unsigned int SizeArray);
      // void OutDataTimeGraf(unsigned int* Data, unsigned int SizeArray);
};

//----------------------------------------------------------------------------------
class CalculateFrequency : public QThread
{
    Q_OBJECT
public:
    CalculateFrequency();
    void run();
    bool accept = false;
    ArraySemples InBuf;
    int ValueFrequency;
    double ValuePeriod;
private:
    int Frequency(uint16_t value);
    double Period(uint16_t value);
signals:
};
//------------------------------------------------------------------------------

class CalculateTimeGrafic : public Calculate
{
    Q_OBJECT
public:
    CalculateTimeGrafic(int BX, int BY, int EX, int EY, unsigned int SR);
    void run();
    CalculateFrequency Frequency;
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

