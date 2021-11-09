#include "spektr.h"

Spektr::Spektr()
{

}

void Spektr::run()
{
    unsigned int sec=0;
    uint16_t* buf = new uint16_t[SamplingRate];
    char* b = reinterpret_cast<char*>(buf);
    ifstream fin;
    fin.open(NameFile.toStdString(), ios::binary); // открыли файл для чтения
    if (!fin.is_open())
    {
       return;
    }
    else
     while(!fin.eof())
     {
         fin.seekg(sec*SamplingRate*2, ios::beg);
         fin.read(b, SamplingRate*2);
         uint16_t* Amp = new uint16_t[SamplingRate];
         DFT(buf, Amp, SamplingRate);
         emit OutResult(sec, Amp);
         sec++;
    }
    fin.close();
    delete buf;
}

int Spektr::DFT(uint16_t *buf, uint16_t *OutBuf, unsigned int n)
{
    const double pi = 3.14159;
    for(unsigned int k=0; k<n; k++)
        {
            int64_t A=0;
            int64_t B=0;
            for(unsigned int m=0; m<n; m++)
            {
                double X = 2* pi*m*k/n;
                //if (X>=0xffffffff){cout << "!!! overflow !!!"<<endl; return -1;}
                double U = static_cast<double>(buf[m]);
                A += static_cast<int64_t>(round(U*cos(X)));
                B += static_cast<int64_t>(round(U*sin(X)));
            }
            A = static_cast<int64_t>(round(A/n));
            B = static_cast<int64_t>(round(B/n));
            OutBuf[k] =  static_cast<uint16_t>(round(sqrt(A*A+B*B)));
        }
    return 1;
}
