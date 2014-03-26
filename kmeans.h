#ifndef KMEANS_H
#define KMEANS_H

#include <QImage>
#include <QVector>

//Структура для описание кластеризуемого пикселя.
//X,Y - координаты пикселя
//ClusterNumber - номер кластера, к которому был отнесен пиксель
struct ClusteredPixel
{
    int X;
    int Y;
    int ClusterNumber;
};

//Структура для описание цветного пикселя
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

    static int const MAX_ITERATION_COUNT = 1000;

    KMeans();

    //Конструктор класса
    //parClusterCount - количество кластеров для поиска
    //parImage - изображение для сегментации
    KMeans(int parClusterCount, QImage parImage);

    int ClusterCount();

    //Кластеризация изображения
    //Возвращает количество итераций
    int Clustering();

    int** Clusters();

    //Нахождение расстояние между центроидом и отдельным пикселем
    //в двумерном пространстве и цветовой схеме RGB
    //parPixel - описание пикселя
    //parClusterCenter - описание центроида кластера
    //Возвращает расстояние между анализируемыми пикселем и центром кластера
    static double Distance(PixelRgb parPixel, PixelRgb parClusterCenter);

    QImage Image();

private:
    //Центры кластеров
    PixelRgb* _ClusterCenters;

    //Количество кластеров
    uint _ClusterCount;

    //Сегментируемое изображение
    QImage _Image;

    //Кластеризуемые пиксели
    int** _Pixels;

    //Проверка, изменились ли координаты центроидов кластеров
    //parOldCenters - старые координаты центроидов
    //parNewCenters - новые координаты центров масс
    bool ClusterCenterChanged(PixelRgb* parOldCenters, PixelRgb* parNewCenters);

    void Init();


};
#endif // KMEANS_H
