#ifndef SPEKTR_H
#define SPEKTR_H
#include <QThread>
#include <iostream>
#include <fstream>
#include <cmath>
#include <QString>
#include "usbcom.h"

using namespace std;

class Spektr : public QThread
{
    Q_OBJECT
public:
    Spektr(int BX, int BY, int EX, int EY, unsigned int SR, uint8_t TF);
    void run();
    void DeleteBufers();
    QString NameFile;
    unsigned int SamplingRate; // частота дискретизации
    int BeginX, BeginY;
    int EndX, EndY;
    unsigned int CountSampling;
    //unsigned int Faza;
    bool TimerWork=false;
    uint16_t* InBuf;
    double* InBufForSpectr;
    unsigned int** OutBuf;
    unsigned int Xmax;
    unsigned int CountBufers;

    void CalculateDFT();
    void CalculateTimeGraf();
    void CalculateFFT(); //uint16_t* IN, unsigned int* Out, int Count
    void (*runFunction)();
    uint8_t TypeFunc;
private:
    int DFT(uint16_t *buf, uint16_t *OutBuf, unsigned int n);


signals:
       void OutResult(unsigned int secund, unsigned int* data, unsigned int SizeArray);
};

#endif // SPEKTR_H
