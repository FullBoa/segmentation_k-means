#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

#include "kmeans.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Обработка кнопки "Загрузить изображение"
    connect(ui->buttonLoadImage, SIGNAL(clicked()), this, SLOT(LoadImage()));

    //Обработка кнопки "Обзор" для выбора изображения
    connect(ui->buttonSelectImage, SIGNAL(clicked()), this, SLOT(SelectImage()));

    connect(ui->buttonSegmentation, SIGNAL(clicked()), this, SLOT(Segmentation()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Загрузка изображения
void MainWindow::LoadImage()
{
    //Если путь к изображению не пустой, то
    if (ui->lineEditImagePath->text() != "")
    {
        //загружаем изображение и
        _ImageSource = QImage(_ImageSourcePath);

        //выводим его на экран
        ui->labelImageSource->setScaledContents(true);
        ui->labelImageSource->setPixmap(QPixmap().fromImage(_ImageSource));
    }
}

//Выбор изображения для обработки
void MainWindow::SelectImage()
{
    //Открываем диалог выбора файла и запоминаем путь к выбранному файлу
    _ImageSourcePath = QFileDialog::getOpenFileName(this, QString::fromUtf8("Исходное изображение"), QDir::homePath());

    //Выводим путь к файлу с помощью lineEdit
    ui->lineEditImagePath->setText(_ImageSourcePath);
}

void MainWindow::Segmentation()
{
    int iterationCount;
    KMeans segmentator(ui->sliderClusterCount->value(), _ImageSource);
    iterationCount = segmentator.Clustering();

    QRgb* colors = new QRgb[segmentator.ClusterCount()];

    for (int k = 0; k < segmentator.ClusterCount(); k++)
    {
        colors[k] = qRgb(32*((k+1) / 4), 32*((k + 2)/ 4),32*((k + 3)/ 4));
    }

    QImage newImage;
    newImage = _ImageSource;

    int numberCluster;
    for (int i = 0; i < segmentator.Image().height(); i++)
    {
        for (int j = 0; j < segmentator.Image().width(); j++)
        {
            numberCluster = segmentator.Clusters()[i][j];
            newImage.setPixel(i,j,colors[numberCluster]);
        }
    }
}
