#ifndef SPEKTR_H
#define SPEKTR_H
#include <QThread>
#include <iostream>
#include <fstream>
#include <cmath>
#include <QString>

using namespace std;

class Spektr : public QThread
{
    Q_OBJECT
public:
    Spektr();
    void run();
    QString NameFile;
    unsigned int SamplingRate; // частота дискретизации
private:
    int DFT(uint16_t *buf, uint16_t *OutBuf, unsigned int n);
signals:
       void OutResult(unsigned int secund,  uint16_t* data);
};

#endif // SPEKTR_H
