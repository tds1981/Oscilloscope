#include "plotgrafic.h"

PlotGrafic::PlotGrafic(int w, int h)
{
    this->setSceneRect(0, 0, w, h);
    BeginX=50;
    BeginY=10;
    EndX= w;
    EndY = h-50;

    ClearPlot();
    ScaleX =10;
}

void  PlotGrafic::ClearPlot()
{
    clear();
    removeItem(nullptr);
    QGraphicsTextItem *TextLevel;
    addLine(BeginX, EndY, EndX, EndY, QPen(Qt::red,3,Qt::SolidLine,Qt::RoundCap));
    addLine(BeginX, BeginY, BeginX, EndY, QPen(Qt::red,3,Qt::SolidLine,Qt::RoundCap));
    int x=BeginX;
    uint8_t i=1;
    while (x<EndX)
    {
        x+=(EndX-BeginX)/CountXSegments;
        addLine(x, BeginY, x, EndY, QPen(Qt::black,1,Qt::DotLine,Qt::RoundCap));
        TextLevel = this->addText(QString::number(ScaleX*i++), QFont ("Times" , 10 , QFont :: Bold));
        TextLevel->setX(x-30); TextLevel->setY(EndY+5);
    }

    int y=BeginY;
    double Volt=5;
    while (y<=EndY)
    {
        addLine(BeginX, y, EndX, y, QPen(Qt::black,1,Qt::DotLine,Qt::RoundCap));
        TextLevel = this->addText(QString::number(Volt,'f', 3)+"V", QFont ("Times" , 10 , QFont :: Bold));
        TextLevel->setX(0); TextLevel->setY(y-5);
        y+=(EndY-BeginY)/8;
        Volt -= 0.625;

    }


    ValueSignal = this->addText("", QFont ("Times" , 10 , QFont ::Thin));
    ValueSignal->setX(10);
    ValueSignal->setY(EndY+25);
}

void PlotGrafic::DrawGrafic()
{
    ClearPlot();
    long double y = BeginY;
    long  double y0 = EndY/2;
    long double A = (EndY-BeginY)/4;

    long double x=0;
    long double fr=100;

    //const double pi = 3.14;
    long double  dx = static_cast<long double>(ScaleX)/((EndX-BeginX)/(CountXSegments*pow(10,static_cast<long double>(PowerSc))));
    int i=BeginX;
    while(i < (EndX-BeginX)-5)
    {
       y =  (EndY-BeginY)/2-A*sin(6.283185f*x*fr);
       addLine(i,static_cast<int>(y0), i+1, static_cast<int>(y), QPen(Qt::green,2,Qt::SolidLine,Qt::RoundCap));
       y0 = y;
       x += dx;
       i++;
    }
}

int PlotGrafic::DrawPoints(int16_t *buf, uint16_t CountPoints)
{
   ClearPlot();
   int k=0;
   long double x=BeginX;
   long double y=BeginY;
   int rise=0, fr=0;
   long double dx = (long double)(EndX-BeginX)/CountPoints;
   //if (((EndX-BeginX)%CountPoints >= 5)&&(dx==0)) dx++;
   while(x<=EndX)
   {
      ValueSignal->setPlainText("k="+QString::number(k)+"   x="+QString::number(x,'f', 3)+"   EndX-BeginX="+QString::number(EndX-BeginX)+"   CountPoints="+QString::number(CountPoints)+"  dx="+QString::number(dx,'f', 3));
      y = BeginY+((EndY-BeginY)/2) - (long double)((EndY-BeginY)*(buf[k]-0x7ff))/0xfff; //
      if (TypeLine) addLine(round(x-dx), round(y0), round(x), round(y), QPen(Qt::green,DensityLine,Qt::SolidLine,Qt::RoundCap));
      else addLine(round(x), round(y), round(x-dx), round(y), QPen(Qt::green,DensityLine,Qt::SolidLine,Qt::RoundCap));

      y0 = y;
    //  if (x*1000/(k+1) >= EndX*1000/CountPoints)
    //  {
         k++;
         // вычисляем количество переходов через максимальное значение
        // if (buf[k]>buf[k-1]) {if (rise==0) fr++; rise++;}
        // if ((buf[k]<buf[k-1])&&(rise>3)) {rise=0;}
    //  }
      x+=dx;

      if (k>=CountPoints) return fr;
   }
   return fr;
}
