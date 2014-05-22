#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QFileDialog>

#include "kmeans.h"
#include "fcm.h"
#include "pcm.h"
#include "pfcm.h"
#include "dialogresult.h"
#include "QDebug"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Обработка кнопки "Загрузить изображение"
    connect(ui->buttonLoadImage, SIGNAL(clicked()), this, SLOT(LoadImage()));

    //Обработка кнопки "Обзор" для выбора изображения
    connect(ui->buttonSelectImage, SIGNAL(clicked()), this, SLOT(SelectImage()));

    //Обработка кнопки "Сегментировать изображение"
    connect(ui->buttonSegmentation, SIGNAL(clicked()), this, SLOT(Segmentation()));

    //Значение количества сегментов на слайдере изменилось
    connect(ui->sliderClusterCount, SIGNAL(valueChanged(int)), this, SLOT(ClusterCountChange(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ClusterCountChange(int parClusterCount)
{
    ui->labelSegmentCount->setText(QString::fromUtf8("Сегментов: ") \
                                   + QString::number(parClusterCount));
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
        ui->labelImageSource->setPixmap(QPixmap().fromImage(_ImageSource).scaled(ui->labelImageSource->size(),
                                                                                 Qt::KeepAspectRatio));
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
    qDebug() << "K-means run";
    int** pixels;
    KMeans segmentator(ui->sliderClusterCount->value(), _ImageSource);
    pixels = segmentator.Clustering();

    qDebug() << "FCM run";
    int** pixelsFCM;
    FCM segmentatorFCM(ui->sliderClusterCount->value(), _ImageSource,0.001,2);
    pixelsFCM = segmentatorFCM.Clustering(20);

    qDebug() << "PCM run";
    int** pixelsPCM;
    PCM segmentatorPCM(ui->sliderClusterCount->value(), _ImageSource,0.001,2);
    pixelsPCM = segmentatorPCM.Clustering(20);

    qDebug() << "PFCM run";
    int** pixelsPFCM;
    PFCM segmentatorPFCM(ui->sliderClusterCount->value(), _ImageSource,0.001,2,2);
    pixelsPFCM = segmentatorPFCM.Clustering(20);

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

    QImage imageFCM;
    imageFCM = _ImageSource;

    QImage imagePCM;
    imagePCM = _ImageSource;

    QImage imagePFCM;
    imagePFCM = _ImageSource;


    int numberCluster;
    for (int i = 0; i < segmentator.Image().width(); i++)
    {
        for (int j = 0; j < segmentator.Image().height(); j++)
        {
            numberCluster = pixels[i][j];
            newImage.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsFCM[i][j];
            imageFCM.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsPCM[i][j];
            imagePCM.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsPFCM[i][j];
            imagePFCM.setPixel(i,j,colors[numberCluster]);
        }
    }

    DialogResult* results = new DialogResult();
    results->ShowResult(segmentator.LastIterationCount(),
                       segmentator.ClusterCount(),
                       newImage,
                        "k-means");

    DialogResult* resultsFCM = new DialogResult();
    resultsFCM->ShowResult(segmentatorFCM.LastIterationCount(),
                       segmentatorFCM.ClusterCount(),
                       imageFCM,
                           "FCM");

    DialogResult* resultsPCM = new DialogResult();
    resultsPCM->ShowResult(segmentatorPCM.LastIterationCount(),
                       segmentatorPCM.ClusterCount(),
                       imagePCM,
                           "PCM");

    DialogResult* resultsPFCM = new DialogResult();
    resultsPFCM->ShowResult(segmentatorPFCM.LastIterationCount(),
                       segmentatorPFCM.ClusterCount(),
                       imagePFCM,
                           "PFCM");
    //ui->labelImageSource->setPixmap(QPixmap().fromImage(newImage).scaled(ui->labelImageSource->size(),
                                         //                               Qt::KeepAspectRatio));
}
