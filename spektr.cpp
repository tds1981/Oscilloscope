#include "spektr.h"
#include <QDebug>

Spektr::Spektr(int BX, int BY, int EX, int EY, unsigned int SR, uint8_t TF)
{
  BeginX = BX;
  BeginY = BY;
  EndX = EX;
  EndY = EY;
  SamplingRate = SR;
  TypeFunc = TF;
  //runFunction = Func;
     tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
}

void Spektr::run()
{
    switch (TypeFunc)
    {
     case 1: CalculateTimeGraf();
        break;
     case 2: CalculateFFT();
        break;

    }
    //CalculateDFT();
   // runFunction();
   // CalculateTimeGraf();
}

const double TwoPi = 6.283185307179586;

void Spektr::CalculateFFT()
{
    int Nvl = 131072;
    int Nft  = 131072;
    double *AVal = InBufForSpectr;
   // double *FTvl = new double;
   static unsigned int sec;

  int i, j, n, m, Mmax, Istp;
  double Tmpr, Tmpi, Wtmp, Theta;
  double Wpr, Wpi, Wr, Wi;
  double *Tmvl;

  n = Nvl * 2; Tmvl = new double[n];

  for (i = 0; i < n; i+=2) {
   Tmvl[i] = 0;
   Tmvl[i+1] = AVal[i/2];
  }

  i = 1; j = 1;
  while (i < n) {
    if (j > i) {
      Tmpr = Tmvl[i]; Tmvl[i] = Tmvl[j]; Tmvl[j] = Tmpr;
      Tmpr = Tmvl[i+1]; Tmvl[i+1] = Tmvl[j+1]; Tmvl[j+1] = Tmpr;
    }
    i = i + 2; m = Nvl;
    while ((m >= 2) && (j > m)) {
      j = j - m; m = m >> 1;
    }
    j = j + m;
  }

  Mmax = 2;
  while (n > Mmax) {
    Theta = -TwoPi / Mmax; Wpi = sin(Theta);
    Wtmp = sin(Theta / 2); Wpr = Wtmp * Wtmp * 2;
    Istp = Mmax * 2; Wr = 1; Wi = 0; m = 1;

    while (m < Mmax) {
      i = m; m = m + 2; Tmpr = Wr; Tmpi = Wi;
      Wr = Wr - Tmpr * Wpr - Tmpi * Wpi;
      Wi = Wi + Tmpr * Wpi - Tmpi * Wpr;

      while (i < n) {
        j = i + Mmax;
        Tmpr = Wr * Tmvl[j] - Wi * Tmvl[j-1];
        Tmpi = Wi * Tmvl[j] + Wr * Tmvl[j-1];

        Tmvl[j] = Tmvl[i] - Tmpr; Tmvl[j-1] = Tmvl[i-1] - Tmpi;
        Tmvl[i] = Tmvl[i] + Tmpr; Tmvl[i-1] = Tmvl[i-1] + Tmpi;
        i = i + Istp;
      }
    }

    Mmax = Istp;
  }


  unsigned int maxX = static_cast<unsigned int>(EndX-BeginX);
  unsigned int* Out = new unsigned int[maxX];
  unsigned int* OutX = new unsigned int[maxX];

  double FTvl_value;
  unsigned int* FTvl = new unsigned int[maxX];
  double FTvl_max=0;
  unsigned int k = 0;
  unsigned int  div = static_cast<unsigned int>(Nft) / maxX;

  i=0;
  while (i < Nft)
  {
      j = i * 2; FTvl_value = 2*sqrt(pow(Tmvl[j],2) + pow(Tmvl[j+1],2))/Nvl;
      if(FTvl_value>FTvl_max) {FTvl_max = FTvl_value; OutX[k]=i;}

      if (i%div == 0)
      {
           FTvl[k] = static_cast<unsigned int>(round(FTvl_max));
           FTvl_max =0;
         // if (FTvl[k]>FTvl_max) FTvl_max = FTvl[k];
          k++;
          if (k>=maxX) k = maxX-1;
      }
      i++;
  }

  for(unsigned int h=0; h<maxX; h++)
  {
      if (FTvl[h]<0xff) Out[h] = (EndY-BeginY) - ((EndY-BeginY)*FTvl[h]/0xff);
      else Out[h] = BeginY;
  }

  sec++;
    qDebug() <<"Hello world";
  emit OutResult(div, Out, OutX, maxX);
  delete []Tmvl;
  delete []InBufForSpectr;
}



void Spektr::CalculateTimeGraf()
{
   unsigned int Xmax = static_cast<unsigned int>(EndX-BeginX);
   unsigned int CountBufers = SizeInBuf/CountSamplingShow;
  // OutBuf = new unsigned int*[CountBufers];

   double dx = (double)Xmax / CountSamplingShow;
   double dk = (double)CountSamplingShow / Xmax;
   double k=0;
   double x=0;
   unsigned int k_index=0;
   unsigned int x_index=0;
   double y=BeginY;
   unsigned int interval = CountSamplingShow*1000 / SamplingRate;
   tmr->setInterval(interval); // Задаем интервал таймера

   for(unsigned int j=0; j<CountBufers; j++)
   {  
    tmr->setSingleShot(true);
    tmr->start();

    unsigned int* OutY = new unsigned int[Xmax];
    x=0;
    x_index=0;
    if (Xmax >= CountSamplingShow)
        while(x_index<Xmax)
        {
        k_index = static_cast<unsigned int>(floor(k));
        if (k_index < SizeInBuf) y = EndY - ((EndY-BeginY)*InBuf[k_index])/0x7ff8; //-0x7ff
        OutY[x_index] = static_cast<unsigned int>(round(y));
        x_index++;
        k+=dk;
        if(k_index>=SizeInBuf) k_index=SizeInBuf-1;
        }
    else
        while(k_index<j*CountSamplingShow)
        {
           x_index = static_cast<unsigned int>(floor(x));
           if (k<SizeInBuf) y = EndY - ((EndY-BeginY)*InBuf[k_index])/0x7ff8; //-0x7ff
           OutY[x_index] = static_cast<unsigned int>(round(y));
           x+=dx;
           k_index++;
           if(k_index>=SizeInBuf) k_index=SizeInBuf-1;
        }

        while(tmr->remainingTime() > 0){};
        emit OutDataTimeGraf(OutY, Xmax);
        tmr->stop();

   }
   delete [] InBuf;
   InBuf = nullptr;
}

/*void Spektr::DeleteBufers()
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
}*/

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
         //emit OutResult(sec, Amp, SamplingRate);
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
