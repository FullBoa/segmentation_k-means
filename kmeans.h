#ifndef KMEANS_H
#define KMEANS_H

#include <QVector>

struct PixelRgb
{
    int X;
    int Y;
    uint Red;
    uint Green;
    uint Blue;
};

class KMeans
{
public:
    KMeans();

    //Нахождение расстояние между центроидом и отдельным пикселем
    //в двумерном пространстве и цветовой схеме RGB
    //parPixel - описание пикселя
    //parClusterCenter - описание центроида кластера
    double Distance(PixelRgb parPixel, PixelRgb parClusterCenter);
};

#endif // KMEANS_H
