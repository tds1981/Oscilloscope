#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QMessageBox>
#include <math.h>
#include <QFile>

#include "plotgrafic.h"
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
    void ReadPort();

    void ResiveDate(uint16_t* data, unsigned int SizeData);
    //void ResiveDateForSpectr(uint16_t* data);
    void ShowDataTimeGraf(unsigned int* Data, unsigned int SizeArray);

    void on_dial_valueChanged(int value);

    void wheelEvent (QWheelEvent * event);
  //  void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void on_pushButton_2_clicked();

    void on_radioButton_2_clicked(bool checked);

    void on_radioButton_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void ChangeTFile();

    void CallFormDFT();
    void on_checkBox_clicked(bool checked);

private:
   void resizeEvent(QResizeEvent * event);
    Ui::MainWindow *ui;
    DFT *frm;
    PlotGrafic *sc;
    CalculateTimeGrafic *Calculate;
    unsigned int CountSamplingShow=1000; //количество семплов выводимых за раз
    uint64_t SizeDataReceived=0;

public:
    UsbCom *usb;

signals:
     void OutDataSpectr(double* buf);

};

#endif // MAINWINDOW_H
