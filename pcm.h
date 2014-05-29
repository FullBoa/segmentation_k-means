#ifndef PCM_H
#define PCM_H

#include "kmeans.h"

class PCM : virtual public KMeans
{
public:

    static double const DEFAULT_DEGREE = 2;

    PCM(int parClusterCount,
        PixelRgb** parPixels,
        int parWidth,
        int parHeight,
        double parDegree = DEFAULT_DEGREE,
        int parMaxIterationCount = DEFAULT_MAX_ITERATION_COUNT,
        double parPrecision = DEFAULT_PRECISION);

    double ObjectiveFunction();

    double TypicalityFunction(int parClusterIndex, int parColumnIndex, int parRowIndex);

protected:

    double* _BandWidth;

    double _DegreeTF;

    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    virtual void Init();

    //Получение новых позиций центроидов
    virtual PixelRgb* NewCenterPositions();

    //Отнесение пикселя к сегментам
    virtual void PixelClustering();

private:
};

#endif // PCM_H
