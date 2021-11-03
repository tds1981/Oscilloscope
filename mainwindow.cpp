#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sc = new PlotGrafic(ui->graphicsView->width()-5, ui->graphicsView->height()-5);
    ui->graphicsView->setScene(sc);
     ui->dial->setValue(10);

    usb = new  UsbCom();
    ui->comboBox->addItem("COM8");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
                 ui->comboBox->addItem(info.portName());

    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
   // tmr->setInterval(TimerInterval); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(TimerEvent())); // Подключаем сигнал таймера к нашему слоту


   // connect(usb, SIGNAL(OutData(uint16_t)), this, SLOT(SetFaza(uint16_t)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
void MainWindow::PrintPoints(int n, char* data)
{
    ui->label->setText(QString::number(n));
    char d[usb->SizeBuferPort];
    memcpy(d, data, sizeof (d));
    //sc->DrawPoints(reinterpret_cast<int16_t*>(data), usb->SizeBuferPort/2);

}*/


void MainWindow::TimerEvent()
{
    static uint16_t Faza;
    static int f;
    int16_t* d = reinterpret_cast<int16_t*>(&usb->PortBuf[Faza]);
    f += sc->DrawPoints(d, TimerInterval*10);
    ui->statusBar->showMessage("TimerInterval: "+QString::number(TimerInterval)+" Faza: "+QString::number(Faza));
    Faza += TimerInterval*20;
    if (Faza >= SizeBuferPort)
    {
        Faza = 0;
        ui->label_2->setText("Частота: "+QString::number(f));
        f=0;
    }
}

void MainWindow::on_dial_valueChanged(int value)
{
    switch (value)
    {
    case 1: sc->ScaleX=1;
    break;
    case 2: sc->ScaleX=1;
    break;
    case 3: sc->ScaleX=2;
    break;
    case 4: sc->ScaleX=2;
    break;
    case 5: sc->ScaleX=4;
    break;
    case 6: sc->ScaleX=4;
    break;
    case 7: sc->ScaleX=5;
    break;
    case 8: sc->ScaleX=5;
    break;
    case 9: sc->ScaleX=10;
    break;
    case 10: sc->ScaleX=10;
    break;
    case 11: sc->ScaleX=20;
    break;
    case 12: sc->ScaleX=20;
    break;
    case 13: sc->ScaleX=50;
    break;
    case 14: sc->ScaleX=50;
    break;
    case 15: sc->ScaleX=80;
    break;
    case 16: sc->ScaleX=80;
    break;
    case 17: sc->ScaleX=100;
    break;
    case 18: sc->ScaleX=100;
    break;
    case 19: sc->ScaleX=200;
    break;
    case 20: sc->ScaleX=200;
    }
    ui->labelScale->setText(QString::number(sc->ScaleX)+" миллисек.");
    TimerInterval=sc->ScaleX*sc->CountXSegments;
    ui->statusBar->showMessage("TimerInterval: "+QString::number(TimerInterval));
    sc->ClearPlot();

}

void MainWindow::on_radioMili_toggled(bool checked)
{

}

void MainWindow::on_radioMicro_toggled(bool checked)
{

}

void MainWindow::on_radioNano_toggled(bool checked)
{

}

void MainWindow::wheelEvent (QWheelEvent *event)
{
   //ui->label->setText(QString::number());
   if ((event->angleDelta().y()>0)&&(ui->dial->value()<20)) ui->dial->setValue(ui->dial->value()+1);
   if ((event->angleDelta().y()<0)&&(ui->dial->value()>0))  ui->dial->setValue(ui->dial->value()-1);
}

void MainWindow::on_pushButton_2_clicked()
{
    usb->NamePort = ui->comboBox->currentText();
    bool pornEn=false;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    if (info.portName()==usb->NamePort) pornEn=true;
    if (!pornEn) {QMessageBox::information(0, "ОШИБКА", "Порт не найден. Подключите кабель"); return;}

    if (!usb->isRunning())
    {
     usb->start(QThread::LowestPriority);
     if (usb->isRunning())//&&usb->serial.isOpen())
     {      tmr->setInterval(TimerInterval); // Задаем интервал таймера
            tmr->start();
            tmr->blockSignals(false);
           // QMessageBox::information(0, "Чтение данных", "Захват данных пошёл");
            //File.setFileName(usb->NameFile);
            //File.open(QIODevice::ReadOnly);
            ui->pushButton_2->setText("Остановить захват данных");
     }
    else QMessageBox::information(0, "ОШИБКА", "Поток не запущен ");
   }
   else
   {
      usb->work = false;
      QMessageBox::information(0, "Захват", "Завершение захвата");
      if (!usb->isRunning())
          ui->pushButton_2->setText("Начать захват данных");
      tmr->blockSignals(true);
      tmr->stop();
      //File.close();
   }

}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    int buf = ui->graphicsView->width();
    ui->graphicsView->resize(event->size().width()-21, event->size().height()-170);
    ui->label->setText("old= "+QString::number(buf)+"  new= "+QString::number( ui->graphicsView->width()));
    sc->setSceneRect(0,0, ui->graphicsView->width()-5, ui->graphicsView->height()-5);
    sc->EndX = ui->graphicsView->width()-5;
    sc->EndY = ui->graphicsView->height()-50;
    sc->ClearPlot();
}



void MainWindow::on_radioButton_2_clicked(bool checked)
{
    sc->TypeLine = !checked;
}

void MainWindow::on_radioButton_clicked(bool checked)
{
   sc->TypeLine = checked;
}

void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{

}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
  sc->DensityLine = arg1;
}
