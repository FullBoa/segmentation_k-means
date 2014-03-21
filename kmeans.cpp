#include "kmeans.h"

#include "math.h"

KMeans::KMeans()
{
}

//Конструктор класса
//parClusterCount - количество кластеров для поиска
//parImage - изображение для сегментации
KMeans::KMeans(uint parClusterCount, QImage parImage)
{
    _ClusterCount = parClusterCount;
    _Image = parImage;

    //Инициализация массива номеров кластеров для каждого пикселя
    _Pixels = new int*[_Image.height()];
    for (int i=0; i < _Image.height(); i++)
    {
        _Pixels[i] = new int[_Image.width()];
    }

    //Инициализация массива центроидов
    _ClusterCenters = new PixelRgb[_ClusterCount];
    for (int i = 0; i < _ClusterCount; i++)
    {
        //переменная для получения целой части числа
        double* intPart;

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
