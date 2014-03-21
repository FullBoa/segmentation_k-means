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

    //Нахождение расстояние между центроидом и отдельным пикселем
    //в двумерном пространстве и цветовой схеме RGB
    //parPixel - описание пикселя
    //parClusterCenter - описание центроида кластера
    //Возвращает расстояние между анализируемыми пикселем и центром кластера
    static double Distance(PixelRgb parPixel, PixelRgb parClusterCenter);

private:
    //Центры кластеров
    QVector<PixelRgb> _CluserCenters;

    //Количество кластеров
    uint _ClusterCount;

    //Сегментируемое изображение
    QImage _Image;

    /*

      ЭТО ВООБЩЕ НУЖНО???

    //Высота изображения в пикселях
    int _ImageHeight;

    //Ширина изображения в пикселях
    int _ImageWidth;*/

    //Кластеризуемые пиксели
    QVector<QVector<ClusteredPixel> > _Pixels;
};

#endif // KMEANS_H
