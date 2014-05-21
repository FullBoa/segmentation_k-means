#ifndef FCM_H
#define FCM_H

#include "kmeans.h"

class FCM : public KMeans
{
public:
    FCM(int parClusterCount, QImage parImage, double epsilon, double parM);

    //Сегментация изображения
    //Возвращает двумерных массив принадлежности пикселя сегментам
    //parMaxIterationCount - максимальное количество итераций при сегментации
    //parDistancePrecision - при изменении позиции центроидов меньше, чем на данное расстояние,
    //считается, что изменений не было.
    int** Clustering(int parMaxIterationCount = MAX_ITERATION_COUNT);

    double MembershipFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

    double ObjectiveFunction();

protected:

    double _Epsilon;

    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    void Init();

    double _M;

    //Получение новых позиций центроидов
    ClusterCenterRgb* NewCenterPositions();
};

#endif // FCM_H
