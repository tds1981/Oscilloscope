#include "dft.h"
#include "ui_dft.h"
#include <QFileDialog>

DFT::DFT(QWidget *parent) :
    QMainWindow(parent),
    uiDFT(new Ui::DFT)
{
    uiDFT->setupUi(this);
    scDFT = new PlotGrafic(uiDFT->graphicsView->width()-15, uiDFT->graphicsView->height()-5, 10000, 10000);
    uiDFT->graphicsView->setScene(scDFT);
    scDFT->ClearPlot();

    SpektrCalcul = new Spektr(scDFT->BeginX, scDFT->BeginY, scDFT->EndX, scDFT->EndY, SamplingRate);
    connect(SpektrCalcul , SIGNAL(OutResult(unsigned int,  uint16_t*)), this, SLOT(ReceiveData(unsigned int,  uint16_t*)));
}

DFT::~DFT()
{
    delete uiDFT;
}

void DFT::ReceiveData(unsigned int sec,  uint16_t* data)
{
     uiDFT->NumberSec->setText("Секунда: "+QString::number(sec));
     scDFT->DrawPoints(data, SpektrCalcul->SamplingRate);
     delete data;
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
