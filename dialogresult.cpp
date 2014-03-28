#include "dialogresult.h"
#include "ui_dialogresult.h"

DialogResult::DialogResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogResult)
{
    ui->setupUi(this);
}

DialogResult::~DialogResult()
{
    delete ui;
}

void DialogResult::ShowResult(int parIterationCount, int parClusterCount, QImage parImage)
{
    this->close();

    ui->labelClusteredImage->setPixmap(QPixmap().fromImage(parImage).scaled(ui->labelClusteredImage->size(),
                                                                        Qt::KeepAspectRatio));


    ui->labelInfo->setText(QString::fromUtf8("Сегментов: ")
                           + QString::number(parClusterCount)
                           + QString::fromUtf8(". Выполено за ")
                           + QString::number(parIterationCount)
                           + QString::fromUtf8(" итераций"));
    this->show();
}
