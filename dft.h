#ifndef DFT_H
#define DFT_H

#include <QMainWindow>
#include <QMessageBox>
#include "plotgrafic.h"
#include "spektr.h"

using namespace std;

namespace Ui {
class DFT;
}

class DFT : public QMainWindow
{
    Q_OBJECT

public:
    explicit DFT(QWidget *parent = nullptr);
    ~DFT();

private slots:
    void on_pushButton_clicked();
    void ReceiveData(unsigned int sec, uint16_t* data);

private:
    void resizeEvent(QResizeEvent * event);
    int DFTcalculation(uint16_t *buf, uint16_t *OutBuf, unsigned int n);
    Ui::DFT *uiDFT;
    PlotGrafic *scDFT;
    Spektr *SpektrCalcul;
};

#endif // DFT_H
