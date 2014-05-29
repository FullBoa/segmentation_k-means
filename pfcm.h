#ifndef PFCM_H
#define PFCM_H

#include "kmeans.h"
#include "fcm.h"
#include "pcm.h"

class PFCM : public FCM, public PCM
{
public:
    PFCM(int parClusterCount,
         PixelRgb** parPixels,
         int parWidth,
         int parHeight,
         double parDegreeMF = FCM::DEFAULT_DEGREE,
         double parDegreeTF = PCM::DEFAULT_DEGREE,
         int parMaxIterationCount = DEFAULT_MAX_ITERATION_COUNT,
         double parPrecision = DEFAULT_PRECISION);

    virtual double ObjectiveFunction();

protected:
    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    virtual void Init();

    //Получение новых позиций центроидов
    virtual PixelRgb* NewCenterPositions();

    virtual void PixelClustering();

private:
};

#endif // PFCM_H
