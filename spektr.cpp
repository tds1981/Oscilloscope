#include "spektr.h"

Spektr::Spektr(int BX, int BY, int EX, int EY, unsigned int SR)
{
  BeginX = BX;
  BeginY = BY;
  EndX = EX;
  EndY = EY;
  SamplingRate = SR;
}

void Spektr::run()
{
    //CalculateDFT();
    CalculateTimeGraf();
}

void Spektr::CalculateTimeGraf()
{
  // if (OutBuf != nullptr) DeleteBufers();
   Xmax = static_cast<unsigned int>(EndX-BeginX);
   CountBufers = SamplingRate/CountSampling;
   OutBuf = new unsigned int*[CountBufers];

   double dx = (double)Xmax / CountSampling;
   double dk = (double)CountSampling / Xmax;
   double k=0;
   double x=0;
   unsigned int k_index=0;
   unsigned int x_index=0;
   double y=BeginY;

   for(unsigned int j=0; j<CountBufers; j++)
   {
    unsigned int* OutY = new unsigned int[Xmax];
    x=0;
    x_index=0;
    if (Xmax >= CountSampling)
        while(x_index<Xmax)
        {
        k_index = static_cast<unsigned int>(floor(k));
        // unsigned int x_index = static_cast<unsigned int>(floor(x));
         if (k_index<SamplingRate) y = EndY - ((EndY-BeginY)*InBuf[k_index])/0x7ff8; //-0x7ff
        OutY[x_index] = static_cast<unsigned int>(round(y));
        x_index++;
        k+=dk;
        if(k_index>=SamplingRate) k_index=SamplingRate-1;
        }
    else
        while(k_index<j*CountSampling)
        {
          // unsigned int k_index = static_cast<unsigned int>(floor(k));
           x_index = static_cast<unsigned int>(floor(x));
           if (k<SamplingRate) y = EndY - ((EndY-BeginY)*InBuf[k_index])/0x7ff8; //-0x7ff
           OutY[x_index] = static_cast<unsigned int>(round(y));
           x+=dx;
           k_index++;
           if(k_index>=SamplingRate) k_index=SamplingRate-1;
        }


    OutBuf[j] = OutY;
   }
   delete [] InBuf;
  // DeleteBufers();

}

void Spektr::DeleteBufers()
{
   unsigned int i=0;
   while(TimerWork){}
   while(i<CountBufers)
     if (OutBuf[i]!=nullptr)
     {
            delete [] OutBuf[i];
            OutBuf[i] = nullptr;
            i++;
     }
   if (!TimerWork){ delete [] OutBuf; OutBuf = nullptr;}
}

void Spektr::CalculateDFT()
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
    delete[] buf;
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
