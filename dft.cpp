#include "dft.h"
#include "ui_dft.h"

DFT::DFT(QWidget *parent) :
    QMainWindow(parent),
    uiDFT(new Ui::DFT)
{
    uiDFT->setupUi(this);
    scDFT = new PlotGrafic(uiDFT->graphicsView->width()-15, uiDFT->graphicsView->height()-5, 10000, 10000);
    uiDFT->graphicsView->setScene(scDFT);
    scDFT->ClearPlot();

    SpektrCalcul = new CalculateFFT(scDFT->BeginX, scDFT->BeginY, scDFT->EndX, scDFT->EndY, SamplingRate); //
    //SpektrCalcul->runFunction = Spektr::CalculateFFT;
    connect(SpektrCalcul, SIGNAL(OutResult(unsigned int,  unsigned int*, unsigned int*, unsigned int)), this, SLOT(ReceiveData(unsigned int,  unsigned int*, unsigned int*, unsigned int)));

    uiDFT->statusbar->showMessage("Форма открыта. Ждём данные");
}

DFT::~DFT()
{
    delete uiDFT;
}
void DFT::ResiveDataUSB(double* buf, unsigned int SizeData)
{
    static int numberArr;
    uiDFT->statusbar->showMessage("Получины данные. Массив № "+ QString::number(++numberArr));
    if (!SpektrCalcul->isRunning())
    {
        SpektrCalcul->InBufForSpectr = buf;
        SpektrCalcul->SizeInBuf = SizeData;
        SpektrCalcul->EndX = scDFT->EndX;
        SpektrCalcul->EndY = scDFT->EndY;
        SpektrCalcul->start(QThread::HighPriority);
    }
    else delete [] buf;
}

void DFT::ReceiveData(unsigned int sec,  unsigned int* data, unsigned int* dataX, unsigned int SizeArray)
{
     uiDFT->NumberSec->setText("Секунда: "+QString::number(sec));
     //scDFT->DrawPoints(data,  SizeArray);
    // scDFT->TypeLine = false;
     scDFT->DrawBuferGrafic(data, SizeArray);
     if (scDFT->ShowArrayX != nullptr) delete[] scDFT->ShowArrayX;
     scDFT->ShowArrayX = dataX;
     delete[] data;
}

void DFT::on_pushButton_clicked()
{
    SpektrCalcul->SamplingRate = 10000;
    SpektrCalcul->NameFile = QFileDialog::getOpenFileName(0, "Выбор файла для ДФП", "", "*.raw");
    //QMessageBox::information(0, "ОШИБКА", "Файл не открылся");
    if (!SpektrCalcul->isRunning())
    {
        SpektrCalcul->start(QThread::NormalPriority);
    }
}


void DFT::resizeEvent(QResizeEvent * event)
{

    uiDFT->graphicsView->resize(event->size().width()-22, event->size().height()-129);

    scDFT->setSceneRect(0,0, uiDFT->graphicsView->width()-5, uiDFT->graphicsView->height()-5);
    scDFT->EndX = uiDFT->graphicsView->width()-5;
    scDFT->EndY = uiDFT->graphicsView->height()-30;
    scDFT->ClearPlot();
}
