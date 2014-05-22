#ifndef PFCM_H
#define PFCM_H

#include "kmeans.h"

class PFCM : public KMeans
{
public:
    PFCM(int parClusterCount, QImage parImage, double parEpsilon, double parMu, double parMw);

    //Сегментация изображения
    //Возвращает двумерных массив принадлежности пикселя сегментам
    //parMaxIterationCount - максимальное количество итераций при сегментации
    //parDistancePrecision - при изменении позиции центроидов меньше, чем на данное расстояние,
    //считается, что изменений не было.
    int** Clustering(int parMaxIterationCount = MAX_ITERATION_COUNT);

    double MembershipFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

    double ObjectiveFunction();

    double TypicalityFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

protected:

    double* _BandWidth;

    double _Epsilon;

    bool _IsClustered;

    double _Mu;

    double _Mw;

    //Получение новых позиций центроидов
    ClusterCenterRgb* NewCenterPositions();

private:
    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    void Init(int parMaxIterationCount);
};

#endif // PFCM_H
