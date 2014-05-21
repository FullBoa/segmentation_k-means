#include "pcm.h"

#include "math.h"

PCM::PCM(int parClusterCount, QImage parImage, double parEpsilon, double parM)
    :KMeans(parClusterCount, parImage)
{
    if (parM > 1)
    {
        _M = parM;
    }
    else
    {
        _M = 2;
    }

    _Epsilon = parEpsilon;
}

double PCM::ObjectiveFunction()
{
    double sum = 0;

    //Цикл по кластерам
    for (int k=0; k<_ClusterCount; k++)
    {
        //Сумма (1-Wk(Xij))^m
        double sumItf = 0;
        //Циклы по i и j - по пикселям изображения
        for (int j=0; j<_Image.height(); j++)
        {
            for (int i=0; i<_Image.width(); i++)
            {
                sum+=pow(TypicalityFunction(k,i,j),_M)
                        *pow(Distance(k,i,j),2);

                sumItf+=pow((1-TypicalityFunction(k,i,j)),_M);
            }
        }

        sum+=_BandWidth[k]*_BandWidth[k]*sumItf;
    }

    return sum;
}

ClusterCenterRgb* PCM::NewCenterPositions()
{
    //Новые координаты центроида
    ClusterCenterRgb* newCenterPositioins = new ClusterCenterRgb[_ClusterCount];

    //Для каждого кластера
    for (int k=0; k < _ClusterCount; k++)
    {
        double numeratorX=0;
        double numeratorY=0;
        double numeratorRed=0;
        double numeratorGreen=0;
        double numeratorBlue=0;

        double denominator=0;

        for (int i=0; i < _Image.width(); i++)
        {
            for (int j=0; j < _Image.height(); j++)
            {
                double u = pow(TypicalityFunction(k,i,j),_M);

                numeratorX+=u*i;
                numeratorY+=u*j;
                numeratorRed+=u*qRed(_Image.pixel(i,j));
                numeratorGreen+=u*qGreen(_Image.pixel(i,j));
                numeratorBlue+=u*qBlue(_Image.pixel(i,j));

                denominator+=u;
            }
        }

        newCenterPositioins[k].X = numeratorX/denominator;
        newCenterPositioins[k].Y = numeratorY/denominator;
        newCenterPositioins[k].Red = numeratorRed/denominator;
        newCenterPositioins[k].Green = numeratorGreen/denominator;
        newCenterPositioins[k].Blue = numeratorBlue/denominator;
    }

    return newCenterPositioins;
}

double PCM::TypicalityFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ)
{
    return 1/(1+pow(Distance(parClusterIndex,parPixelIndexI,parPixelIndexJ)/_BandWidth[parClusterIndex],2/(_M-1)));
}
