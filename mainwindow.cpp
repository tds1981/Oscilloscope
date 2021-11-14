#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sc = new PlotGrafic(ui->graphicsView->width()-5, ui->graphicsView->height()-5, 10, 5);
    ui->graphicsView->setScene(sc);
    //ui->dial->setValue(ui->dial->value());

    Calculate = new Spektr(sc->BeginX, sc->BeginY, sc->EndX, sc->EndY, SamplingRate, 1);
   // Calculate->runFunction = Calculate->CalculateTimeGraf;
    usb = new  UsbCom();
    ui->comboBox->addItem("COM8");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
                 ui->comboBox->addItem(info.portName());

    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
   // tmr->setInterval(TimerInterval); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(TimerEvent())); // Подключаем сигнал таймера к нашему слоту
    connect(usb, SIGNAL(OutData(uint16_t*)), this, SLOT(ResiveDate(uint16_t*)));

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

    frm = new DFT(this);
   // connect(this, SIGNAL(OutDataSpectr(double*)), frm, SLOT(ResiveDataUSB(double*)));
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
    frm->show();
}

void MainWindow::ResiveDate(uint16_t* data)
{
   // if (frm != nullptr)
   // {
        double* DataForSpectr = new double[SamplingRate+31072];
        for(unsigned int j=0; j<SamplingRate; j++) DataForSpectr[j]=static_cast<double>(data[j]);
        //if (frm->DataSamples == nullptr) frm->DataSamples =  DataForSpectr;
        emit OutDataSpectr(DataForSpectr);
   // }

   if (!Calculate->isRunning())
   {
       Calculate->CountSampling = SamplingRate*TimerInterval/1000;
       Calculate->InBuf = data;
       Calculate->EndX = sc->EndX;
       Calculate->EndY = sc->EndY;
       if ((Calculate->OutBuf != DateBufs)&&(Calculate->OutBuf != nullptr)) Calculate->DeleteBufers();
       Calculate->start(QThread::HighPriority);
       if (tmr->signalsBlocked())
       {
           NumberBuf=0;
           tmr->setInterval(TimerInterval); // Задаем интервал таймера
           tmr->start();
           tmr->blockSignals(false);
       }
   }
   else delete [] data;
}

void MainWindow::TimerEvent()
{ 
    Calculate->TimerWork = true;
    if ((NumberBuf == 0)&&(Calculate->OutBuf != nullptr))
    {     
         DateBufs = Calculate->OutBuf;
        // Calculate->OutBuf = nullptr;
         CountBufs = Calculate->CountBufers;
         Size1Buf = Calculate->Xmax;
    }

    if (DateBufs != nullptr)
      if (DateBufs[NumberBuf] != nullptr)
      {
        sc->DrawBuferGrafic(DateBufs[NumberBuf], Size1Buf);
        ui->statusBar->showMessage("  Timer Interval: "+QString::number(TimerInterval)+ "  CountBufers: "+QString::number(Calculate->CountBufers)+ + "  CountSampling: "+QString::number(Calculate->CountSampling)+ "  Xmax: "+QString::number(Calculate->Xmax) + " NumberBuf: "+QString::number(NumberBuf));
        delete [] DateBufs[NumberBuf];
        DateBufs[NumberBuf]=nullptr;
        NumberBuf++;
        if (NumberBuf>=CountBufs) {NumberBuf=0; delete [] DateBufs; DateBufs=nullptr;}
     }
     Calculate->TimerWork = false;
}

void MainWindow::on_dial_valueChanged(int value)
{
    tmr->blockSignals(true);
    usb->blockSignals(true);
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
    }
    else
    {
        ui->labelScale->setText(QString::number(Scale/1000)+" МИЛЛИсекунд");
    }
    while(Calculate->TimerWork){};
    while(Calculate->isRunning()){};
    if((DateBufs != nullptr)&&(NumberBuf!=0))
    {
       for(unsigned int i=0; i<CountBufs; i++)
        if(DateBufs[i] != nullptr) {delete [] DateBufs[i]; DateBufs[i]=nullptr;}
       delete [] DateBufs;
       DateBufs = nullptr;
    }
    if (Calculate->OutBuf != nullptr) Calculate->DeleteBufers();
    TimerInterval=(Scale * sc->CountXSegments)/1000; // микросекунды в миллисекунды
    ui->statusBar->showMessage("TimerInterval: "+QString::number(TimerInterval));
    usb->blockSignals(false);
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
     usb->start(QThread::HighPriority);
     if (usb->isRunning())//&&usb->serial.isOpen())
     {
         tmr->blockSignals(true);
         // tmr->setInterval(TimerInterval); // Задаем интервал таймера
           // tmr->start();
           // tmr->blockSignals(false);
           // QMessageBox::information(0, "Чтение данных", "Захват данных пошёл");
            ui->pushButton_2->setText("Остановить захват данных");
     }
    else QMessageBox::information(0, "ОШИБКА", "Поток не запущен ");
   }
   else
   {
      usb->work = false;
      tmr->blockSignals(true);
      tmr->stop();
      QMessageBox::information(0, "Захват", "Завершение захвата");
      if (!usb->isRunning())
          ui->pushButton_2->setText("Начать захват данных");
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
