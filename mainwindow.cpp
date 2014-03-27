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
        //Red
        if (k % 4 == 0)
        {
            colors[k] = qRgb(255 - 32 * k / 4, 0, 0);
        }
        //White, gray, black
        else if ((k + 1) % 4 == 0)
        {
            colors[k] = qRgb(255 - 32 * (k+1) / 4, 255 - 32 * (k+1) / 4, 255 - 32 * (k+1) / 4);
        }
        //Blue
        else if ((k + 2) % 4 == 0)
        {
            colors[k] = qRgb(0, 0, 255 - 32 * (k+2) / 4);
        }
        //Green
        else
        {
            colors[k] = qRgb(0, 255 - 32 * (k+3) / 4, 0);
        }
    }

    QImage newImage;
    newImage = _ImageSource;

    int numberCluster;
    for (int i = 0; i < segmentator.Image().width(); i++)
    {
        for (int j = 0; j < segmentator.Image().height(); j++)
        {
            numberCluster = segmentator.Clusters()[i][j];
            newImage.setPixel(i,j,colors[numberCluster]);
        }
    }

    ui->labelImageSource->setScaledContents(true);
    ui->labelImageSource->setPixmap(QPixmap().fromImage(newImage));
}
