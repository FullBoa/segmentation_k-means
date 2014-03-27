#include "kmeans.h"
#include <QDebug>

#include "math.h"

KMeans::KMeans()
{
}

//Конструктор класса
//parClusterCount - количество кластеров для поиска
//parImage - изображение для сегментации
KMeans::KMeans(int parClusterCount, QImage parImage)
{
    if (parClusterCount > 0)
    {
        _ClusterCount = parClusterCount;
    }
    _Image = parImage;

    _Pixels = NULL;
}

int KMeans::ClusterCount()
{
    return _ClusterCount;
}

int KMeans::Clustering()
{
    Init();
    qDebug() << "Height: " << _Image.height();
    qDebug() << "Width: " << _Image.width();

    bool done = false;
    int iterationCount = 0;
    do
    {
        iterationCount++;
        qDebug() << "iteration " << iterationCount;

        //Распределение пикселей по кластерам

        //Минимальное расстояние до центроида
        double minDistance;

        //Номер ближайшего центроида
        int numberNearestCluster;

        //Проходим по всем пикселям
        for (int i = 0; i < _Image.width(); i++)
        {
            if (i % 100 == 0)
            {
                qDebug() << "column " << i;
            }
            for (int j = 0; j < _Image.height(); j++)
            {                
                //Текущий обрабатываемый пиксель
                PixelRgb currentPixel;
                currentPixel.X = i;
                currentPixel.Y = j;
                currentPixel.Red = qRed(_Image.pixel(i,j));
                currentPixel.Green = qGreen(_Image.pixel(i,j));
                currentPixel.Blue = qBlue(_Image.pixel(i,j));

                //Ишем ближайший центроид
                minDistance = Distance(currentPixel,_ClusterCenters[0]);
                numberNearestCluster = 0;

                double currentDistance;
                for(int k = 1; k < _ClusterCount; k++)
                {
                    currentDistance = Distance(currentPixel, _ClusterCenters[k]);
                    if (currentDistance < minDistance)
                    {
                        numberNearestCluster = k;
                        minDistance = currentDistance;
                    }
                }

                //Приписываем пиксель кластеру
                _Pixels[i][j] = numberNearestCluster;
            }
        }

        //Вычисление новых центров масс кластеров.

        //Новые координаты центроида
        PixelRgb* newCenterPositioin = new PixelRgb[_ClusterCount];

        //Пикселей в кластере
        int* pixelInCluster = new int[_ClusterCount];

        //Инициализация новых центроидов
        for (int k = 0; k < _ClusterCount; k++)
        {
            pixelInCluster[k] = 0;

            newCenterPositioin[k].X = 0;
            newCenterPositioin[k].Y = 0;
            newCenterPositioin[k].Red = 0;
            newCenterPositioin[k].Green = 0;
            newCenterPositioin[k].Blue = 0;
        }


        //Расчет новых значений центроидов
        int numberCluster;
        for (int i = 0; i < _Image.width(); i++)
        {
            for (int j = 0; j < _Image.height(); j++)
            {
                numberCluster =_Pixels[i][j];
                pixelInCluster[numberCluster]++;

                newCenterPositioin[numberCluster].X += i;
                newCenterPositioin[numberCluster].Y += j;
                newCenterPositioin[numberCluster].Red += qRed(_Image.pixel(i,j));
                newCenterPositioin[numberCluster].Green += qGreen(_Image.pixel(i,j));
                newCenterPositioin[numberCluster].Blue += qBlue(_Image.pixel(i,j));
            }
        }

        for (int k = 0; k < _ClusterCount; k++)
        {
            newCenterPositioin[k].X /= pixelInCluster[k];
            newCenterPositioin[k].Y /= pixelInCluster[k];
            newCenterPositioin[k].Red /= pixelInCluster[k];
            newCenterPositioin[k].Green /= pixelInCluster[k];
            newCenterPositioin[k].Blue /= pixelInCluster[k];
        }

        if (ClusterCenterChanged(_ClusterCenters, newCenterPositioin))
        {
            delete _ClusterCenters;
            _ClusterCenters = newCenterPositioin;
        }
        else
        {
            done = true;
        }
    }
    while(!done && (iterationCount < MAX_ITERATION_COUNT));

    return iterationCount;
}

int** KMeans::Clusters()
{
    return _Pixels;
}

//Проверка, изменились ли координаты центроидов кластеров
//parOldCenters - старые координаты центроидов
//parNewCenters - новые координаты центров масс
bool KMeans::ClusterCenterChanged(PixelRgb *parOldCenters, PixelRgb *parNewCenters)
{
    bool changed = false;

    for (int k = 0; (k < _ClusterCount) && !changed; k++)
    {
        //TODO: убрать магическое число!!!
        if (Distance(parOldCenters[k], parNewCenters[k]) > 10)
        {
            changed = true;
        }
    }

    return changed;
}

//Нахождение расстояние между центроидом и отдельным пикселем
//в двумерном пространстве и цветовой схеме RGB
//parPixel - описание пикселя
//parClusterCenter - описание центроида кластера
//Возвращает расстояние между анализируемыми пикселем и центром кластера
double KMeans::Distance(PixelRgb parPixel, PixelRgb parClusterCenter)
{
    return sqrt((parClusterCenter.X - parPixel.X)*(parClusterCenter.X - parPixel.X)
                + (parClusterCenter.Y - parPixel.Y)*(parClusterCenter.Y - parPixel.Y)
                + (parClusterCenter.Red - parPixel.Red)*(parClusterCenter.Red - parPixel.Red)
                + (parClusterCenter.Green - parPixel.Green)*(parClusterCenter.Green - parPixel.Green)
                + (parClusterCenter.Blue - parPixel.Blue)*(parClusterCenter.Blue - parPixel.Blue));
}

QImage KMeans::Image()
{
    return _Image;
}

void KMeans::Init()
{
    //Инициализация массива номеров кластеров для каждого пикселя
    _Pixels = new int*[_Image.width()];
    for (int i=0; i < _Image.width(); i++)
    {
        _Pixels[i] = new int[_Image.height()];
    }

    //Инициализация массива центроидов
    _ClusterCenters = new PixelRgb[_ClusterCount];
    for (int i = 0; i < _ClusterCount; i++)
    {
        //переменная для получения целой части числа
        double* intPart = new double;
        *intPart = 0;

        modf(qrand() / (double) INT_MAX * _Image.width(), intPart);
        _ClusterCenters[i].X = (int) *intPart;

        modf(qrand() / (double) INT_MAX * _Image.height(), intPart);
        _ClusterCenters[i].Y = (int) *intPart;\

        QRgb pixel = _Image.pixel(_ClusterCenters[i].X, _ClusterCenters[i].Y);
        _ClusterCenters[i].Red = qRed(pixel);
        _ClusterCenters[i].Green = qGreen(pixel);
        _ClusterCenters[i].Blue = qBlue(pixel);
    }
}
