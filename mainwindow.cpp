#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonLoadImage, SIGNAL(clicked()), this, SLOT(LoadImage()));
    connect(ui->buttonSelectImage, SIGNAL(clicked()), this, SLOT(SelectImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadImage()
{
    if (ui->lineEditImagePath->text() != "")
    {
        QPixmap pixmap(_ImagePath);
        ui->labelImageSource->setScaledContents(true);
        ui->labelImageSource->setPixmap(pixmap);
    }
}

void MainWindow::SelectImage()
{
    _ImagePath = QFileDialog::getOpenFileName(this, QString::fromUtf8("Исходное изображение"), QDir::homePath());
    ui->lineEditImagePath->setText(_ImagePath);
}
