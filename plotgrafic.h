#ifndef PLOTGRAFIC_H
#define PLOTGRAFIC_H
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QResizeEvent>
#include <QString>
#include <math.h>

class PlotGrafic : public QGraphicsScene
{
    Q_OBJECT

public:
    int EndX, EndY;
    int ScaleX; //
    int8_t PowerSc;
    int8_t CountXSegments = 10; // количество делений по оси X
    bool TypeLine=true;
    uint8_t DensityLine=1;
    PlotGrafic(int w, int h);
    void  DrawGrafic();
    int  DrawPoints(int16_t *buf, uint16_t CountPoints);
    void  ClearPlot();
signals:

public slots:

private:
        QGraphicsTextItem *ValueSignal;
        long double y0 = 0;
        int BeginX, BeginY;
};

#endif // PLOTGRAFIC_H
