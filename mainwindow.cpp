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


   // connect(tmr, SIGNAL(timeout()), this, SLOT(TimerEvent())); // Подключаем сигнал таймера к нашему слоту
    connect(usb, SIGNAL(OutData(uint16_t*, unsigned int)), this, SLOT(ResiveDate(uint16_t*, unsigned int)));
    connect(Calculate, SIGNAL(OutDataTimeGraf(unsigned int*, unsigned int)), this, SLOT(ShowDataTimeGraf(unsigned int*, unsigned int)));

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

void MainWindow::ResiveDateForSpectr(uint16_t* data)
{
    // if (frm != nullptr)
    // {
         double* DataForSpectr = new double[SamplingRate+31072];
         for(unsigned int j=0; j<SamplingRate; j++) DataForSpectr[j]=static_cast<double>(data[j]);
         //if (frm->DataSamples == nullptr) frm->DataSamples =  DataForSpectr;
         emit OutDataSpectr(DataForSpectr);
    // }

}
void MainWindow::ResiveDate(uint16_t* data, unsigned int SizeData)
{
   static unsigned int CountElements;

   if (!Calculate->isRunning())
   {
     if (SizeData >= CountSamplingShow)
     {
       Calculate->CountSamplingShow = CountSamplingShow;
       Calculate->InBuf = data;
       Calculate->SizeInBuf = SizeData;
       Calculate->EndX = sc->EndX;
       Calculate->EndY = sc->EndY;
       Calculate->start(QThread::HighPriority);
     }
     else
     {
        if (Calculate->InBuf != nullptr)
                Calculate->InBuf = new uint16_t[CountSamplingShow];
        for(unsigned int i=0;i<SizeData;i++) Calculate->InBuf[CountElements++]=data[i];
        delete [] data;
        if (CountElements>=CountSamplingShow)
        {
           Calculate->SizeInBuf = CountElements;
           CountElements=0;
           Calculate->CountSamplingShow = CountSamplingShow;
           Calculate->EndX = sc->EndX;
           Calculate->EndY = sc->EndY;
           Calculate->start(QThread::HighPriority);
        }
     }
   }
   else delete [] data;
}
 void MainWindow::ShowDataTimeGraf(unsigned int* Data, unsigned int SizeArray)
{
    sc->DrawBuferGrafic(Data, SizeArray);
     ui->statusBar->showMessage(" SizeInBuf: "+QString::number(Calculate->SizeInBuf)+ " Show X max: "+QString::number(SizeArray)+ " CountSamplingShow: "+QString::number(Calculate->CountSamplingShow));
    delete [] Data;
}

/*void MainWindow::TimerEvent()
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

        delete [] DateBufs[NumberBuf];
        DateBufs[NumberBuf]=nullptr;
        NumberBuf++;
        if (NumberBuf>=CountBufs) {NumberBuf=0; delete [] DateBufs; DateBufs=nullptr;}
     }
     Calculate->TimerWork = false;
}*/

void MainWindow::on_dial_valueChanged(int value)
{
    ui->dial->setEnabled(false);
   // bool StateUsb = usb->work;
   /* if (StateUsb)
    {
        usb->blockSignals(true);
        usb->work = false;
    }*/

    unsigned int Scale=0; // микросекунд в делении
    switch (value)
    {
    case 1: Scale = SamplingRate/1000;
    break;
    case 2: Scale = SamplingRate/1000;
    break;
    case 3: Scale = SamplingRate/500;
    break;
    case 4: Scale = SamplingRate/500;
    break;
    case 5: Scale = SamplingRate/200;
    break;
    case 6: Scale = SamplingRate/200;
    break;
    case 7: Scale = SamplingRate/100;
    break;
    case 8: Scale = SamplingRate/100;
    break;
    case 9: Scale = SamplingRate/50;
    break;
    case 10: Scale = SamplingRate/50;
    break;
    case 11: Scale = SamplingRate/20;
    break;
    case 12: Scale = SamplingRate/20;
    break;
    case 13: Scale = SamplingRate/10;
    break;
    case 14: Scale = SamplingRate/10;
    break;
    case 15: Scale = SamplingRate/5;
    break;
    case 16: Scale = SamplingRate/5;
    break;
    case 17: Scale = SamplingRate/2;
    break;
    case 18: Scale = SamplingRate/2;
    break;
    case 19: Scale = SamplingRate;
    break;
    case 20: Scale = SamplingRate;

    }

    if (Scale<1000)
    {
        ui->labelScale->setText(QString::number(Scale)+" МИКРОсекунд");
    }
    else
    {
        ui->labelScale->setText(QString::number(Scale/1000)+" МИЛЛИсекунд");
    }

    CountSamplingShow = Scale;
    ui->statusBar->showMessage("CountSamplingShow: "+QString::number(CountSamplingShow));

    //while (usb->isRunning());
  //  if (Scale > SamplingRate/10) SizeDataOut = SamplingRate;
  //  else SizeDataOut = SamplingRate/10;
/*
    if (StateUsb)
    {
        usb->work = true;
        usb->start(QThread::HighPriority);
        if (usb->isRunning()) usb->blockSignals(false);
    }*/
    ui->dial->setEnabled(true);
}


void MainWindow::wheelEvent (QWheelEvent *event)
{
   if ((event->x()>ui->groupBox->x())&&(event->x()<ui->groupBox->x()+ui->groupBox->width())
      && (event->y()>ui->groupBox->y())&&(event->y()<ui->groupBox->y()+ui->groupBox->height())
      && ui->dial->isEnabled())
   {
    if ((event->angleDelta().y()>0)&&(ui->dial->value()<20)) ui->dial->setValue(ui->dial->value()+1);
    if ((event->angleDelta().y()<0)&&(ui->dial->value()>0))  ui->dial->setValue(ui->dial->value()-1);
   }
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
         //tmr->blockSignals(true);
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
