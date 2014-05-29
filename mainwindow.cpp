#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QFileDialog>
#include "QList"
#include "QPoint"

#include "kmeans.h"
#include "fcm.h"
#include "pcm.h"
#include "pfcm.h"
#include "svmefc.h"
#include "dialogresult.h"
#include "QDebug"

#include "svm.h"


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
    //Подготовка данных для сегментации
    int clusterCount = ui->sliderClusterCount->value();

    int width = _ImageSource.width();
    int height = _ImageSource.height();

    PixelRgb** image = new PixelRgb*[width];
    for (int i=0; i < width; i++)
    {
        image[i] = new PixelRgb[height];
    }

    //Нормализация значений характеристик пикселей
    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            image[i][j].X = (double)i/(double)width;
            image[i][j].Y = (double)j/(double)height;
            image[i][j].Red = (double)qRed(_ImageSource.pixel(i,j))/255.0;
            image[i][j].Green = (double)qGreen(_ImageSource.pixel(i,j))/255.0;
            image[i][j].Blue = (double)qBlue(_ImageSource.pixel(i,j))/255.0;
        }
    }

    qDebug() << "K-means run";
    int** pixels;
    KMeans segmentator(clusterCount,image,width, height);
    pixels = segmentator.Clustering();

    PixelRgb* clusterCentersKM = segmentator.ClusterCenters();

    qDebug() << "FCM run";
    int** pixelsFCM;
    FCM segmentatorFCM(clusterCount,image,width,height);
    segmentatorFCM.SetClusterCenters(clusterCentersKM);
    pixelsFCM = segmentatorFCM.Clustering();

    PixelRgb* clusterCentersFCM = segmentatorFCM.ClusterCenters();

    qDebug() << "PCM run";
    int** pixelsPCM;
    PCM segmentatorPCM(clusterCount,image,width,height);
    segmentatorPCM.SetClusterCenters(clusterCentersFCM);
    pixelsPCM = segmentatorPCM.Clustering();

    qDebug() << "PFCM run";
    int** pixelsPFCM;
    PFCM segmentatorPFCM(clusterCount,image,width,height);
    segmentatorPFCM.SetClusterCenters(clusterCentersFCM);
    pixelsPFCM = segmentatorPFCM.Clustering();


   // qDebug() << "SVM run";
    //SVM parameters
    svm_parameter parameters;
    parameters.svm_type = C_SVC;
    parameters.kernel_type = POLY;
    parameters.degree = 2;
    parameters.gamma = 1;
    parameters.coef0 = 3;

    parameters.cache_size = 64;
    parameters.eps = 0.001;
    parameters.C = 5;
    parameters.nr_weight = 0;
    parameters.shrinking = 0;
    parameters.probability = 0;

    int** pixelsCSPA;
    int*** labels = new int**[3];

    labels[0] = pixelsFCM;
    labels[1] = pixelsPCM;
    labels[2] = pixelsPFCM;

    PixelRgb** clusterCenters = new PixelRgb*[3];
    clusterCenters[0] = segmentatorFCM.ClusterCenters();
    clusterCenters[1] = segmentatorPCM.ClusterCenters();
    clusterCenters[2] = segmentatorPFCM.ClusterCenters();

    pixelsCSPA = SVMeFC::SegmentationCSPA(image,
                                          width,
                                          height,
                                          labels,
                                          clusterCount,
                                          clusterCenters,
                                          3,
                                          parameters);

    int** pixelsSVM;
    pixelsSVM = SVMeFC::SegmentationEncemble(image,
                                             width,
                                             height,
                                             labels,
                                             3,
                                             parameters);




    QRgb* colors = new QRgb[clusterCount];

    for (int k = 0; k < clusterCount; k++)
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

    QImage imageSVM;
    imageSVM = _ImageSource;

    QImage imageFCM;
    imageFCM = _ImageSource;

    QImage imagePCM;
    imagePCM = _ImageSource;

    QImage imagePFCM;
    imagePFCM = _ImageSource;

    QImage imageCSPA;
    imageCSPA = _ImageSource;


    int numberCluster;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            numberCluster = pixels[i][j];
            newImage.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsFCM[i][j];
            imageFCM.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsPCM[i][j];
            imagePCM.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsPFCM[i][j];
            imagePFCM.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsSVM[i][j];
            imageSVM.setPixel(i,j,colors[numberCluster]);

            numberCluster = pixelsCSPA[i][j];
            imageCSPA.setPixel(i,j,colors[numberCluster]);
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

    DialogResult* resultsCSPA = new DialogResult();
    resultsCSPA->ShowResult(1,
                       clusterCount,
                       imageCSPA,
                           "CSPA");

    DialogResult* resultsSVM = new DialogResult();
    resultsSVM->ShowResult(1,
                       ui->sliderClusterCount->value(),
                       imageSVM,
                           "SVMeFC");
    //ui->labelImageSource->setPixmap(QPixmap().fromImage(newImage).scaled(ui->labelImageSource->size(),
                                         //                               Qt::KeepAspectRatio));
}
