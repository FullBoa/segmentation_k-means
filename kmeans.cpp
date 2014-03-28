#include "kmeans.h"
#include <QDebug>

#include "math.h"

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

    _LastIterationCount = 0;
}

//Получение количества кластеров
int KMeans::ClusterCount()
{
    return _ClusterCount;
}

//Сегментация изображения
//Возвращает двумерных массив принадлежности пикселя сегментам
//parMaxIterationCount - максимальное количество итераций при сегментации
//parDistancePrecision - при изменении позиции центроидов меньше, чем на данное расстояние,
//считается, что изменений не было.
int **KMeans::Clustering(int parMaxIterationCount,
                         double parDistancePrecision)
{
    //Инициализация центроидов и массива принадлежности пикселей сегментам
    Init();

    //флаг окончания сегментации
    bool done = false;

    //Количество пройденных итераций
    int iterationCount = 0;
    do
    {
        iterationCount++;

        //Распределение пикселей по кластерам
        PixelClustering();

        //Вычисление новых центров масс кластеров.
        PixelRgb* newCenterPositions;
        newCenterPositions = NewCenterPositions();

        if (ClusterCenterChanged(_ClusterCenters, newCenterPositions))
        {
            delete _ClusterCenters;
            _ClusterCenters = newCenterPositions;
        }
        else
        {
            done = true;
        }
    }
    while(!done && (iterationCount < parMaxIterationCount));

    _LastIterationCount = iterationCount;

    return _Pixels;
}

//Проверка, изменились ли координаты центроидов кластеров
//parOldCenters - старые координаты центроидов
//parNewCenters - новые координаты центров масс
//parDistancePrecision - точность определения изменения расстояния.
//Перемещение центроидов меньше, чем на данное расстояние, изменением не считается.
bool KMeans::ClusterCenterChanged(PixelRgb *parOldCenters, PixelRgb *parNewCenters, double parDistancePrecision)
{
    bool changed = false;

    for (int k = 0; (k < _ClusterCount) && !changed; k++)
    {
        if (Distance(parOldCenters[k], parNewCenters[k]) > parDistancePrecision)
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

//Получение сегментируемого изображения
QImage KMeans::Image()
{
    return _Image;
}

//Инициализация массивов центроидов и пикселей сегментируемого изображения
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

//Получение количества итераций при последний сегментации
int KMeans::LastIterationCount()
{
    return _LastIterationCount;
}

//Получение новых позиций центроидов
PixelRgb* KMeans::NewCenterPositions()
{
    //Новые координаты центроида
    PixelRgb* newCenterPositioins = new PixelRgb[_ClusterCount];

    //Пикселей в кластере
    int* pixelInCluster = new int[_ClusterCount];

    //Инициализация новых центроидов
    for (int k = 0; k < _ClusterCount; k++)
    {
        pixelInCluster[k] = 0;

        newCenterPositioins[k].X = 0;
        newCenterPositioins[k].Y = 0;
        newCenterPositioins[k].Red = 0;
        newCenterPositioins[k].Green = 0;
        newCenterPositioins[k].Blue = 0;
    }


    //Расчет новых значений центроидов
    int numberCluster;
    for (int i = 0; i < _Image.width(); i++)
    {
        for (int j = 0; j < _Image.height(); j++)
        {
            numberCluster =_Pixels[i][j];
            pixelInCluster[numberCluster]++;

            newCenterPositioins[numberCluster].X += i;
            newCenterPositioins[numberCluster].Y += j;
            newCenterPositioins[numberCluster].Red += qRed(_Image.pixel(i,j));
            newCenterPositioins[numberCluster].Green += qGreen(_Image.pixel(i,j));
            newCenterPositioins[numberCluster].Blue += qBlue(_Image.pixel(i,j));
        }
    }

    for (int k = 0; k < _ClusterCount; k++)
    {
        if (pixelInCluster[k] != 0)
        {
            newCenterPositioins[k].X /= pixelInCluster[k];
            newCenterPositioins[k].Y /= pixelInCluster[k];
            newCenterPositioins[k].Red /= pixelInCluster[k];
            newCenterPositioins[k].Green /= pixelInCluster[k];
            newCenterPositioins[k].Blue /= pixelInCluster[k];
        }
    }

    return newCenterPositioins;
}

//Отнесение пикселя к сегментам
void KMeans::PixelClustering()
{
    //Минимальное расстояние до центроида
    double minDistance;

    //Номер ближайшего центроида
    int numberNearestCluster;

    //Проходим по всем пикселям
    for (int i = 0; i < _Image.width(); i++)
    {
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
}
