#ifndef KMEANS_H
#define KMEANS_H

#include <QImage>

//Структура для описание цветного пикселя
struct PixelRgb
{
    int X;
    int Y;
    uint Red;
    uint Green;
    uint Blue;
};

//Структура для описания центра кластера цветных пикселей
struct ClusterCenterRgb
{
    double X;
    double Y;
    double Red;
    double Green;
    double Blue;
};

class KMeans
{
public:

    //Максимальное количество итерации при сегментации
    static int const MAX_ITERATION_COUNT = 100;

    //Значение изменения рассояния по умолчанию, при изменении позиции цетроидов меньше которого,
    //считается, что изменений не было
    static double const DEFAULT_DISTANCE_PRECISION = 5;

    //Конструктор класса
    //parClusterCount - количество кластеров для поиска
    //parImage - изображение для сегментации
    KMeans(int parClusterCount, QImage parImage);

    //Получение количества кластеров
    int ClusterCount();

    //Сегментация изображения
    //Возвращает двумерных массив принадлежности пикселя сегментам
    //parMaxIterationCount - максимальное количество итераций при сегментации
    //parDistancePrecision - при изменении позиции центроидов меньше, чем на данное расстояние,
    //считается, что изменений не было.
    int** Clustering(int parMaxIterationCount = MAX_ITERATION_COUNT,
                     double parDistancePrecision = DEFAULT_DISTANCE_PRECISION);

    static double Distance(ClusterCenterRgb parFirstCenter,
                            ClusterCenterRgb parSecondCenter);

    double Distance(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

    //Нахождение расстояние между центроидом и отдельным пикселем
    //в пространстве (X,Y,R,G,B)
    //parPixel - описание пикселя
    //parClusterCenter - описание центроида сегмета
    //Возвращает расстояние между анализируемыми пикселем и центром сегмента
    static double Distance(PixelRgb parPixel, ClusterCenterRgb parClusterCenter);

    //Получение сегментируемого изображения
    QImage Image();

    //Получение количества итераций при последний сегментации
    int LastIterationCount();

    //Задание количества сегментов, на которое нужно будет разбить изображение
    void SetClusterCount(int parClusterCount);

protected:
    //Центры кластеров
    ClusterCenterRgb* _ClusterCenters;

    //Количество кластеров
    uint _ClusterCount;

    //Сегментируемое изображение
    QImage _Image;

    //Количество итераций при последней сегментации
    int _LastIterationCount;

    //Кластеризуемые пиксели
    int** _Pixels;


    //Проверка, изменились ли координаты центроидов кластеров
    //parOldCenters - старые координаты центроидов
    //parNewCenters - новые координаты центров масс
    //parDistancePrecision - точность определения изменения расстояния.
    //Перемещение центроидов меньше, чем на данное расстояние, изменением не считается.
    bool ClusterCenterChanged(ClusterCenterRgb *parOldCenters,
                              ClusterCenterRgb *parNewCenters,
                              double parDistancePrecision = DEFAULT_DISTANCE_PRECISION);

    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    void Init();

    //Получение новых позиций центроидов
    ClusterCenterRgb* NewCenterPositions();

    //Отнесение пикселя к сегментам
    void PixelClustering();


};
#endif // KMEANS_H
