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
    qDebug() << "K-means run";
    int** pixels;
    KMeans segmentator(ui->sliderClusterCount->value(), _ImageSource);
    pixels = segmentator.Clustering();

    qDebug() << "FCM run";
    int** pixelsFCM;
    FCM segmentatorFCM(ui->sliderClusterCount->value(), _ImageSource,0.001,2);
    pixelsFCM = segmentatorFCM.Clustering(20);

    ClusterCenterRgb* clusterCentersFCM = segmentatorFCM.ClusterCenters();

    qDebug() << "PCM run";
    int** pixelsPCM;
    PCM segmentatorPCM(ui->sliderClusterCount->value(), _ImageSource,0.001,2);
    segmentatorPCM.SetClusterCenters(clusterCentersFCM);
    pixelsPCM = segmentatorPCM.Clustering(20);

    qDebug() << "PFCM run";
    int** pixelsPFCM;
    PFCM segmentatorPFCM(ui->sliderClusterCount->value(), _ImageSource,0.001,2,2);
    segmentatorPFCM.SetClusterCenters(clusterCentersFCM);
    pixelsPFCM = segmentatorPFCM.Clustering(20);


    qDebug() << "SVM run";

    //Разделение выборки на обучающую и тестовую
    QList<double>* distances= new QList<double>[ui->sliderClusterCount->value()]();

    //Получаем расстояния от пикселей до соответсвующих центров кластеров
    for (int i=0; i<_ImageSource.width(); i++)
    {
        for (int j=0; j<_ImageSource.height(); j++)
        {
            distances[pixels[i][j]].append(segmentator.Distance(pixels[i][j],i,j));
        }
    }

    for (int k=0; k<ui->sliderClusterCount->value(); k++)
    {
        qSort(distances[k]);
    }

    double* averageDistants = new double[ui->sliderClusterCount->value()];
    for (int k=0; k<ui->sliderClusterCount->value(); k++)
    {
        qDebug() << "in cluster " << k;
        qDebug() << "min distance: " << distances[k].at(0);
        qDebug() << "max distance: " << distances[k].at(distances[k].size()-1);
        averageDistants[k] = distances[k].at((distances[k].size()-1)/2);
        qDebug() << "average distance: " << averageDistants[k];
    }

    int** pixelsSVM = new int*[_ImageSource.width()];
    for (int i=0; i < _ImageSource.width(); i++)
    {
        pixelsSVM[i] = new int[_ImageSource.height()];
    }

    QList<svm_node*> trainList;
    QList<int> labelList;

    int trainSetSize = 0;

    for (int i=0; i<_ImageSource.width(); i++)
    {
        for (int j=0; j<_ImageSource.height(); j++)
        {
            //Точка расположена близко к центру кластера
            if (segmentator.Distance(pixels[i][j],i,j) <= averageDistants[pixels[i][j]])
            {
                pixelsSVM[i][j] = pixels[i][j];

                svm_node* node = new svm_node[6]; //5 под размерность и 1 под (индекс -1)
                node[0].index = 1;
                node[0].value =(double)i/(double)_ImageSource.width();

                node[1].index = 2;
                node[1].value =(double)j/(double)_ImageSource.height();

                node[2].index=3;
                node[2].value=(double)qRed(_ImageSource.pixel(i,j))/(double)255;

                node[3].index=4;
                node[3].value=(double)qGreen(_ImageSource.pixel(i,j))/(double)255;

                node[4].index=5;
                node[4].value=(double)qBlue(_ImageSource.pixel(i,j))/(double)255;

                node[5].index=-1;

                trainList.append(node);
                labelList.append(pixels[i][j]);

                trainSetSize++;
            }
            //Точка расположена далеко от центра кластера
            else
            {
                pixelsSVM[i][j] = -1;
            }
        }
    }

    int testSetSize = _ImageSource.width()*_ImageSource.height() - trainSetSize;

    svm_problem trainProblem;
    trainProblem.l = trainSetSize;
    trainProblem.y = new double[trainSetSize];
    trainProblem.x = new svm_node*[trainSetSize];

    for(int i=0; i<trainSetSize; i++)
    {
        trainProblem.y[i]=labelList.at(i);
        trainProblem.x[i]=trainList.at(i);
    }

    qDebug() << "train set size: " << trainProblem.l;
    qDebug() << "test set size: " << testSetSize;

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

    qDebug() << "Check svm parameters: " << svm_check_parameter(&trainProblem, &parameters);
    if (svm_check_parameter(&trainProblem, &parameters) == NULL)
    {
        svm_model* model=svm_train(&trainProblem,&parameters);


        for (int i=0; i<_ImageSource.width(); i++)
        {
            for (int j=0; j<_ImageSource.height(); j++)
            {
                if (pixelsSVM[i][j] == -1)
                {
                    svm_node* node = new svm_node[6]; //5 под размерность и 1 под (индекс -1)
                    node[0].index = 1;
                    node[0].value =(double)i/(double)_ImageSource.width();

                    node[1].index = 2;
                    node[1].value =(double)j/(double)_ImageSource.height();

                    node[2].index=3;
                    node[2].value=(double)qRed(_ImageSource.pixel(i,j))/(double)255;

                    node[3].index=4;
                    node[3].value=(double)qGreen(_ImageSource.pixel(i,j))/(double)255;

                    node[4].index=5;
                    node[4].value=(double)qBlue(_ImageSource.pixel(i,j))/(double)255;

                    node[5].index=-1;

                    pixelsSVM[i][j] = (int)svm_predict(model,node);

                    delete node;
                }
            }
        }
    }


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

    QImage imageSVM;
    imageSVM = _ImageSource;

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

            numberCluster = pixelsSVM[i][j];
            imageSVM.setPixel(i,j,colors[numberCluster]);
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

    DialogResult* resultsSVM = new DialogResult();
    resultsSVM->ShowResult(1,
                       ui->sliderClusterCount->value(),
                       imageSVM,
                           "SVM");
    //ui->labelImageSource->setPixmap(QPixmap().fromImage(newImage).scaled(ui->labelImageSource->size(),
                                         //                               Qt::KeepAspectRatio));
}
