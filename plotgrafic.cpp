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
    uint8_t i=0;
    while (i<=CountXSegments)
    {
        addLine(x, BeginY, x, EndY, QPen(Qt::black,1,Qt::DotLine,Qt::RoundCap));
        TextLevel = this->addText(QString::number(ScaleX*i), QFont ("Times" , 10 , QFont :: Bold));
        if(i != CountXSegments) TextLevel->setX(x-20);
        else TextLevel->setX(x-40);
        TextLevel->setY(EndY+5);
        x+=(EndX-BeginX)/CountXSegments;
        i++;
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

double PlotGrafic::DrawPoints(uint16_t *buf, uint16_t CountPoints)
{
   ClearPlot();
   int k=0;
   double x=BeginX;
   double y=BeginY;
   int rise=0, fall=0, k1,i=0;
   int period[10]={0};
   double dx = (long double)(EndX-BeginX)/CountPoints;
   //if (((EndX-BeginX)%CountPoints >= 5)&&(dx==0)) dx++;
   while(x<=EndX)
   {
     // ValueSignal->setPlainText("k="+QString::number(k)+"   x="+QString::number(x,'f', 3)+"   EndX-BeginX="+QString::number(EndX-BeginX)+"   CountPoints="+QString::number(CountPoints)+"  dx="+QString::number(dx,'f', 3));
      y = EndY - ((EndY-BeginY)*buf[k])/0x7ff8; //-0x7ff
      ValueSignal->setPlainText("buf= "+QString::number(buf[k])+"   y="+QString::number(y,'f', 3));
      if (TypeLine) addLine(round(x-dx), round(y0), round(x), round(y), QPen(Qt::green,DensityLine,Qt::SolidLine,Qt::RoundCap));
      else addLine(round(x), round(y), round(x-dx), round(y), QPen(Qt::green,DensityLine,Qt::SolidLine,Qt::RoundCap));

      y0 = y;
      x+=dx;
      k++;
      // вычисляем количество переходов через максимальное значение
      if (buf[k]>buf[k-1]) {rise++;}
      if ((buf[k]<buf[k-1])&&(rise>3)) {rise=0; k1=k-1;}

      if (buf[k]<buf[k-1]) {fall++;}
      if ((buf[k]>buf[k-1])&&(fall>3))
      {
          fall=0;
          period[i++]=k-k1-1;
          if (i>=10) i=0;
       /*   if (k-k1>0)
          {
             if (i!=0)
             {
                 double filtr=((k-k1)*i)/ksum;
                if ((filtr>0.9)&&(filtr<1.1)) {ksum+=k-k1;i++;}
             }
            else {ksum+=k-k1;i++;}
          }*/
      }
      if (k>=CountPoints) break;
   }
   int sum=0; uint8_t n=0;
   QString s = "period: ";
   for(uint8_t j=1; j<10; j++)
   {
       s+=QString::number(period[j])+" ";
     if ((abs(period[j-1]-period[j])<=1)&&(period[j]!=0)) {sum+=period[j]; n++;}
   }
   //ValueSignal->setPlainText(s);
   if (n!=0) return sum/n;
   else return -1;
}
