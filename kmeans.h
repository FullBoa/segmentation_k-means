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
    KMeans();

    //Конструктор класса
    //parClusterCount - количество кластеров для поиска
    //parImage - изображение для сегментации
    KMeans(uint parClusterCount, QImage parImage);

    void Clustering();

    //Проверка, изменились ли координаты центроидов кластеров
    //parOldCenters - старые координаты центроидов
    //parNewCenters - новые координаты центров масс
    bool ClusterCenterChanged(PixelRgb* parOldCenters, PixelRgb* parNewCenters);

    //Нахождение расстояние между центроидом и отдельным пикселем
    //в двумерном пространстве и цветовой схеме RGB
    //parPixel - описание пикселя
    //parClusterCenter - описание центроида кластера
    //Возвращает расстояние между анализируемыми пикселем и центром кластера
    static double Distance(PixelRgb parPixel, PixelRgb parClusterCenter);

    void Init();

private:
    //Центры кластеров
    PixelRgb* _ClusterCenters;

    //Количество кластеров
    uint _ClusterCount;

    //Сегментируемое изображение
    QImage _Image;

    PixelRgb* _LastCenterPositions;

    //Кластеризуемые пиксели
    //QVector<QVector<ClusteredPixel> > _Pixels;
    int** _Pixels;


};
#endif // KMEANS_H
