#ifndef PCM_H
#define PCM_H

#include "kmeans.h"

class PCM : public KMeans
{
public:
    PCM(int parClusterCount, QImage parImage, double parEpsilon, double parM);

    //Сегментация изображения
    //Возвращает двумерных массив принадлежности пикселя сегментам
    //parMaxIterationCount - максимальное количество итераций при сегментации
    //parDistancePrecision - при изменении позиции центроидов меньше, чем на данное расстояние,
    //считается, что изменений не было.
    int** Clustering(int parMaxIterationCount = MAX_ITERATION_COUNT);

    double ObjectiveFunction();

    double TypicalityFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

protected:

    double* _BandWidth;

    double _Epsilon;

    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    void Init();

    double _M;

    //Получение новых позиций центроидов
    ClusterCenterRgb* NewCenterPositions();
};

#endif // PCM_H
