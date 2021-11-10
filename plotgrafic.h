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
    PlotGrafic(int w, int h, unsigned int MX, unsigned int MY);
    void   DrawGrafic();
    double DrawPoints(uint16_t *buf, uint16_t CountPoints);
    void   ClearPlot();
    void   DrawBuferGrafic(unsigned int* Buf, unsigned int SizeBuf);

    int EndX, EndY;
    int BeginX, BeginY;
    unsigned int MaxX; // максимальное значение по оси X
    unsigned int MaxY;
    uint8_t CountXSegments = 10; // количество делений по оси X
    bool TypeLine=true;
    uint8_t DensityLine=2;
signals:

public slots:

private:
        QGraphicsTextItem *ValueSignal;
        double y0 = 0;
};

#endif // PLOTGRAFIC_H
