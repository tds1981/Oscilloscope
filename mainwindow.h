#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QMessageBox>
#include <math.h>
#include <QTimer>
#include <QFile>

#include "plotgrafic.h"
#include "usbcom.h"
#include "spektr.h"
#include "dft.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void TimerEvent();

    void ResiveDate(uint16_t* data);

    void on_dial_valueChanged(int value);

    void wheelEvent (QWheelEvent * event);
  //  void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void on_pushButton_2_clicked();

    void on_radioButton_2_clicked(bool checked);

    void on_radioButton_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void ChangeTFile();

    void CallFormDFT();
private:
   void resizeEvent(QResizeEvent * event);
    Ui::MainWindow *ui;
    DFT *frm;
    PlotGrafic *sc;
    Spektr *Calculate;
    QTimer *tmr;
    uint32_t TimerInterval=10;

    unsigned int** DateBufs;
    unsigned int CountBufs;
    unsigned int Size1Buf;
    unsigned int NumberBuf=0;
    //QFile File;
public:
     UsbCom *usb;

signals:
     void OutDataSpectr(double* buf);

};

#endif // MAINWINDOW_H
