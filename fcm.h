#ifndef FCM_H
#define FCM_H

#include "kmeans.h"

class FCM : virtual public KMeans
{
public:

    static double const DEFAULT_DEGREE = 2;

    FCM(int parClusterCount,
        PixelRgb** parPixels,
        int parWidth,
        int parHeight,
        double parDegree = DEFAULT_DEGREE,
        int parMaxIterationCount = DEFAULT_MAX_ITERATION_COUNT,
        double parPrecision = DEFAULT_PRECISION);

    double MembershipFunction(int parClusterIndex, int parColumnIndex, int parRowIndex);

    virtual double ObjectiveFunction();

protected:

    double _DegreeMF;

    //Получение новых позиций центроидов
    virtual PixelRgb* NewCenterPositions();

    //Отнесение пикселя к сегментам
    virtual void PixelClustering();
};

#endif // FCM_H
