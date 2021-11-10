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
    Spektr(int BX, int BY, int EX, int EY, unsigned int SR);
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
    unsigned int** OutBuf;
    unsigned int Xmax;
    unsigned int CountBufers;

private:
    int DFT(uint16_t *buf, uint16_t *OutBuf, unsigned int n);
    void CalculateDFT();
    void CalculateTimeGraf();

signals:
       void OutResult(unsigned int secund,  uint16_t* data);
};

#endif // SPEKTR_H
