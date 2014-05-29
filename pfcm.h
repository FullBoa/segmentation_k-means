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

    //Сегментация изображения
    //Возвращает двумерных массив принадлежности пикселя сегментам
    //parMaxIterationCount - максимальное количество итераций при сегментации
    //parDistancePrecision - при изменении позиции центроидов меньше, чем на данное расстояние,
    //считается, что изменений не было.
 //   int** Clustering(int parMaxIterationCount = MAX_ITERATION_COUNT);

   // double MembershipFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

    virtual double ObjectiveFunction();

    //double TypicalityFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ);

protected:
    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    virtual void Init();

    //Получение новых позиций центроидов
    virtual PixelRgb* NewCenterPositions();

    virtual void PixelClustering();

private:
};

#endif // PFCM_H
