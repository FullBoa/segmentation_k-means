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
