#ifndef FCM_H
#define FCM_H

#include "kmeans.h"

class FCM : public KMeans
{
public:
    FCM(int parClusterCount, QImage parImage, double epsilon, double parM);

    int** Clustering(int parMaxIterationCount = MAX_ITERATION_COUNT);

    ClusterCenterRgb* GetClusterCenters(int parMaxIterationCount = MAX_ITERATION_COUNT);

    double MembershipFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

    double ObjectiveFunction();

protected:

    double _Epsilon;

    bool _IsClustered;

    double _M;

    //Получение новых позиций центроидов
    ClusterCenterRgb* NewCenterPositions();

private:
    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    void Init();
};

#endif // FCM_H
