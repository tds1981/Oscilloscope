#include "spektr.h"
#include <QDebug>

Calculate::Calculate() //int BX, int BY, int EX, int EY, unsigned int SR
{

}
void Calculate::SetParametrs(int BX, int BY, int EX, int EY, unsigned int SR)
{
    BeginX = BX;
    BeginY = BY;
    EndX = EX;
    EndY = EY;
    SamplingRate = SR;
}

bool Calculate::CheckCountElementsInDEQ()
{
    unsigned int CountElements=0;
    for(int j=0; j<deqSamples.size(); j++) CountElements+=deqSamples.at(j).sizeData;
    if (CountElements<CountSamplingShow)  return false;
    else return true;
}

ArraySemples Calculate::CreateInBuf(bool Del_accept)
{
    ArraySemples InBuf;
    if (deqSamples.empty()) {InBuf.sizeData=0; return InBuf;}
    if (deqSamples.back().sizeData < CountSamplingShow)
    {
        if (!CheckCountElementsInDEQ()) {InBuf.sizeData=0; return InBuf;}

        unsigned int CountElements=0;
        InBuf.data = new uint16_t[CountSamplingShow];
        InBuf.sizeData =CountSamplingShow;
        InBuf.Delete = Del_accept;
        while (CountElements<CountSamplingShow)
        {
            for(int h=0; h<deqSamples.back().sizeData; h++)
            {
                if(CountElements+h >= CountSamplingShow) break;
                InBuf.data[CountElements+h]=deqSamples.back().data[h];
            }
            CountElements+=deqSamples.back().sizeData;
            if (deqSamples.back().Delete) delete [] deqSamples.back().data;
            deqSamples.pop_back();
        }
    }
    else
    {
        InBuf = deqSamples.back();
        deqSamples.pop_back();
    }
    return  InBuf;
}

//------------------------- class CalculateTimeGrafic --------------------------------------------
CalculateTimeGrafic::CalculateTimeGrafic(int BX, int BY, int EX, int EY, unsigned int SR)
{
   SetParametrs(BX, BY, EX, EY, SR);
   tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной*/
   //Frequency = new CalculateFrequency();
}

void CalculateTimeGrafic::run()
{
    ArraySemples InBuf = CreateInBuf(!Frequency.accept); // флаг разрешающий удаление буфера выставляем в зависимости от того вычисляется далее частота или нет
    if (InBuf.sizeData == 0) return;

    unsigned int Xmax = static_cast<unsigned int>(EndX-BeginX);
    unsigned int CountBufers = InBuf.sizeData/CountSamplingShow;

    double dx = (double)Xmax / CountSamplingShow;
    double dk = (double)CountSamplingShow / Xmax;
    double k=0;
    double x=0;
    unsigned int k_index=0;
    unsigned int x_index=0;
    double y=BeginY;
    unsigned int interval = CountSamplingShow*1000 / SamplingRate;
    tmr->setInterval(interval); // Задаем интервал таймера
     qDebug() <<"Timer interval: "<<interval;

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
              if (k_index < InBuf.sizeData) y = EndY - ((EndY-BeginY)*InBuf.data[k_index])/0x7ff8; //-0x7ff
              OutY[x_index] = static_cast<unsigned int>(round(y));
              x_index++;
              k+=dk;
              if(k_index>=InBuf.sizeData) break; //k_index=SizeInBuf-1;
         }
     else
         for(unsigned int h=0; h<CountSamplingShow; h++)
         {
           // x_index = static_cast<unsigned int>(floor(x));
            if (k_index<InBuf.sizeData) y = EndY - ((EndY-BeginY)*InBuf.data[k_index])/0x7ff8; //-0x7ff
            if (x_index<Xmax) OutY[x_index] = static_cast<unsigned int>(round(y));
            //x+=dx;
            if (h % (CountSamplingShow / Xmax) ==0) x_index++;
            k_index++;
            if(k_index>=InBuf.sizeData) break; // k_index=SizeInBuf-1;
         }

         while(tmr->remainingTime() > 0){};
         emit OutDataTimeGraf(OutY, Xmax);
         tmr->stop();
         if(k_index>=InBuf.sizeData) break;
    }
    if (InBuf.Delete) delete [] InBuf.data;
    else
    {
      InBuf.Delete = true;
      Frequency.InBuf=InBuf;
      Frequency.start(QThread::LowPriority);
    }
}
//-----------------------class CalculateFrequency ----------------
CalculateFrequency::CalculateFrequency()
{
}

int CalculateFrequency::Frequency(uint16_t value)
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

double CalculateFrequency::Period(uint16_t value)
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

void CalculateFrequency::run()
{
    double  periodBuf[10];
    unsigned int j=0;
    for(unsigned int i=0; i<InBuf.sizeData; i++)
    {
        int f = Frequency(InBuf.data[i]);
        double pBuf = Period(InBuf.data[i]);
        if (f != -1) ValueFrequency = f;
        if (pBuf != -1) periodBuf[j++] = pBuf;
        if (j >= 10) j=0;
    }
    double delta=abs(periodBuf[0]-periodBuf[1]);
    unsigned int k=0;
    for(unsigned int i=1; i<9; i++)
        if (delta > abs(periodBuf[i]-periodBuf[i+1]))
                {delta = abs(periodBuf[i]-periodBuf[i+1]); k=i;}

     ValuePeriod = periodBuf[k];
     if (InBuf.Delete) delete [] InBuf.data;
}

//--------------------- class CalculateFFT ----------------------------------
CalculateFFT::CalculateFFT(int BX, int BY, int EX, int EY, unsigned int SR)
{
   SetParametrs(BX, BY, EX, EY, SR);
}

void CalculateFFT::run()
{
    if (!CheckCountElementsInDEQ()) return;

    int Nvl = (int)CountSamplingShow;
    int Nft = (int)CountSamplingShow;
   // double *AVal = InBufForSpectr;
   // double *FTvl = new double;
   static unsigned int sec;

  int i, j, n, m, Mmax, Istp;
  double Tmpr, Tmpi, Wtmp, Theta;
  double Wpr, Wpi, Wr, Wi;
  double *Tmvl;

  n = Nvl * 2; Tmvl = new double[n];
  unsigned int ind=0;
  for (i = 0; i < n; i+=2) {
   Tmvl[i] = 0;
   Tmvl[i+1] = deqSamples.back().dataDouble[ind++]; // AVal[i/2];
   if (ind>=deqSamples.back().sizeData)
   {
       ind=0;
       if (deqSamples.back().dataDouble != nullptr) delete [] deqSamples.back().dataDouble;
       if (!deqSamples.empty()) deqSamples.pop_back();
   }
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
  double f;
  double Kfr = static_cast<double>(SamplingRate)/static_cast<double>(CountSamplingShow*div);
  i=0;
  while (i < Nft)
  {
      j = i * 2; FTvl_value = 2*sqrt(pow(Tmvl[j],2) + pow(Tmvl[j+1],2))/Nvl;
      //if(FTvl_value>FTvl_max)
      //  {
            FTvl_max += FTvl_value/static_cast<double>(div);
            f += static_cast<double>(i)*Kfr;
       // }

      if (i%div == 0)
      {
           FTvl[k] = static_cast<unsigned int>(round(FTvl_max));
           OutX[k]= static_cast<unsigned int>(round(f));
           FTvl_max =0;
           f=0;
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
  emit OutResult(sec, Out, OutX, maxX);
  delete []Tmvl;

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

/*
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
*/
