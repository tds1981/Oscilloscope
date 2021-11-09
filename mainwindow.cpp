#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dft.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sc = new PlotGrafic(ui->graphicsView->width()-5, ui->graphicsView->height()-5, 10, 5);
    ui->graphicsView->setScene(sc);
    //ui->dial->setValue(ui->dial->value());

    usb = new  UsbCom();
    ui->comboBox->addItem("COM8");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
                 ui->comboBox->addItem(info.portName());

    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
   // tmr->setInterval(TimerInterval); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(TimerEvent())); // Подключаем сигнал таймера к нашему слоту

    QAction* TypeFile = new QAction("TypeFile", 0);
    TypeFile->setText("Тип файла: wav или rav");
    TypeFile->setShortcut(QKeySequence("CTRL+T"));
    TypeFile->setToolTip("TypeFile");
    TypeFile->setStatusTip("Выбор типа файла: wav или rav");
    TypeFile->setWhatsThis("Выбор типа файла: wav / rav");
    connect(TypeFile, SIGNAL(triggered()), SLOT(ChangeTFile()));
    ui->menu_2->addAction(TypeFile);

    QAction* DFT_Form = new QAction("DFT", 0);
    DFT_Form->setText("Дискретное преобразование Фурье");
    DFT_Form->setShortcut(QKeySequence("CTRL+F"));
    DFT_Form->setToolTip("DFT");
    DFT_Form->setStatusTip("Дискретное преобразование Фурье");
    connect(DFT_Form, SIGNAL(triggered()), SLOT(CallFormDFT()));
    ui->menu_3->addAction(DFT_Form);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ChangeTFile()
{
         if (usb->TypeFile[0]=='w')
         {   usb->TypeFile[0]='r';  usb->TypeFile[2]='w';
             ui->statusBar->showMessage("Тип сохраняемого файла: raw");
             QMessageBox::information(0, "Смена типа сохраняемого файла", "Тип сохраняемого файла: raw");
         }
         else
         {   usb->TypeFile[0]='w'; usb->TypeFile[2]='v';
             QMessageBox::information(0, "Смена типа сохраняемого файла", "Тип сохраняемого файла: wav");
            ui->statusBar->showMessage("Тип сохраняемого файла: wav");
         }
}

void MainWindow::CallFormDFT()
{
   DFT *frm = new DFT();
   frm->show();
}

void MainWindow::TimerEvent()
{
   // static int f;
    unsigned int CountSampling = SamplingRate*TimerInterval/1000;
    double N = sc->DrawPoints(&usb->PortBuf[Faza], CountSampling);
    double fr;
    if (N!=0) fr = 10000/(2*N);
    ui->label_2->setText("Частота: "+QString::number(fr,'f', 3));
    ui->statusBar->showMessage("Timer Interval: "+QString::number(TimerInterval)+ "CountSampling: "+QString::number(CountSampling) + " Faza: "+QString::number(Faza));
    Faza += CountSampling;
    if (Faza >= SamplingRate)
    {
        Faza = 0;
        //ui->label_2->setText("Частота: "+QString::number(f/2));
       // f=0;
    }
}

void MainWindow::on_dial_valueChanged(int value)
{
    tmr->blockSignals(true);
    tmr->stop();
    unsigned int Scale=0; // микросекунд в делении
    switch (value)
    {
    case 1: Scale=100;
    break;
    case 2: Scale=100;
    break;
    case 3: Scale=200;
    break;
    case 4: Scale=200;
    break;
    case 5: Scale=500;
    break;
    case 6: Scale=500;
    break;
    case 7: Scale=1000;
    break;
    case 8: Scale=1000;
    break;
    case 9: Scale=2000;
    break;
    case 10: Scale=2000;
    break;
    case 11: Scale=5000;
    break;
    case 12: Scale=5000;
    break;
    case 13: Scale=10000;
    break;
    case 14: Scale=10000;
    break;
    case 15: Scale=20000;
    break;
    case 16: Scale=20000;
    break;
    case 17: Scale=50000;
    break;
    case 18: Scale=50000;
    break;
    case 19: Scale=100000;
    break;
    case 20: Scale=100000;

    }

    if (Scale<1000)
    {
        ui->labelScale->setText(QString::number(Scale)+" МИКРОсекунд");
        sc->MaxX = Scale * sc->CountXSegments;
    }
    else
    {
        ui->labelScale->setText(QString::number(Scale/1000)+" МИЛЛИсекунд");
        sc->MaxX = (Scale/1000) * sc->CountXSegments;
    }
    TimerInterval=(Scale* sc->CountXSegments)/1000; // микросекунды в миллисекунды

    Faza = 0;
    ui->statusBar->showMessage("TimerInterval: "+QString::number(TimerInterval));
    sc->ClearPlot();
    tmr->setInterval(TimerInterval); // Задаем интервал таймера
    if (usb->isRunning())
    {
           tmr->start();
           tmr->blockSignals(false);
    }
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
   }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    ui->graphicsView->resize(event->size().width()-21, event->size().height()-170);
    //ui->label->setText("old= "+QString::number(buf)+"  new= "+QString::number( ui->graphicsView->width()));
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

void MainWindow::on_spinBox_valueChanged(int arg1)
{
  sc->DensityLine = arg1;
}
