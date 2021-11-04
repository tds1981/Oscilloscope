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

    //void PrintPoints(int n, char* data);

    void on_dial_valueChanged(int value);

    void on_radioMili_toggled(bool checked);

    void on_radioMicro_toggled(bool checked);

    void on_radioNano_toggled(bool checked);

    void wheelEvent (QWheelEvent * event);
  //  void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void on_pushButton_2_clicked();

    void on_radioButton_2_clicked(bool checked);

    void on_radioButton_clicked(bool checked);

   // void on_spinBox_valueChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_MainWindow_iconSizeChanged(const QSize &iconSize);

private:
   void resizeEvent(QResizeEvent * event);
    Ui::MainWindow *ui;
    PlotGrafic *sc;
    QTimer *tmr;
    uint16_t TimerInterval=100;
    //QFile File;
public:
     UsbCom *usb;

};

#endif // MAINWINDOW_H
