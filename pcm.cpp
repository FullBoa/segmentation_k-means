#include "pcm.h"

#include "math.h"
#include "fcm.h"

#include "QDebug"

PCM::PCM(int parClusterCount,
         PixelRgb **parPixels,
         int parWidth,
         int parHeight,
         double parDegree,
         int parMaxIterationCount,
         double parPrecision)
    : KMeans (parClusterCount, parPixels, parWidth,parHeight,parMaxIterationCount,parPrecision)
{

    if (parDegree > 1)
    {
        _DegreeTF = parDegree;
    }
    else
    {
        _DegreeTF = DEFAULT_DEGREE;
    }
}

void PCM::Init()
{
    //Инициализация массива номеров кластеров для каждого пикселя
    _PixelLabels = new int*[_Width];
    for (int i=0; i < _Width; i++)
    {
        _PixelLabels[i] = new int[_Height];
    }

    FCM fcm(_ClusterCount,_Pixels, _Width, _Height, _DegreeTF, _MaxIterationCount, _Precision);

    if (_ClusterCenters == NULL)
    {
        fcm.FindClusterCenters();
        _ClusterCenters = fcm.ClusterCenters();
    }
    else
    {
        fcm.SetClusterCenters(_ClusterCenters);
    }

    _BandWidth = new double[_ClusterCount];

    for (int k=0; k<_ClusterCount; k++)
    {
        double numerator=0;
        double denominator=0;

        for (int i=0; i<_Width; i++)
        {
            for (int j=0; j<_Height; j++)
            {
                double mf = pow(fcm.MembershipFunction(k,i,j),_DegreeTF);

                numerator+=mf*pow(fcm.Distance(k,i,j),2);
                denominator+=mf;
            }
        }

        _BandWidth[k]=sqrt(numerator/denominator);
    }
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
        for (int j=0; j<_Height; j++)
        {
            for (int i=0; i<_Width; i++)
            {
                sum+=pow(TypicalityFunction(k,i,j),_DegreeTF)
                        *pow(Distance(k,i,j),2);

                sumItf+=pow((1-TypicalityFunction(k,i,j)),_DegreeTF);
            }
        }

        sum+=_BandWidth[k]*_BandWidth[k]*sumItf;
    }

    return sum;
}

PixelRgb *PCM::NewCenterPositions()
{
    //Новые координаты центроида
    PixelRgb* newCenterPositioins = new PixelRgb[_ClusterCount];

    //Для каждого кластера
    for (int k=0; k < _ClusterCount; k++)
    {
        double numeratorX=0;
        double numeratorY=0;
        double numeratorRed=0;
        double numeratorGreen=0;
        double numeratorBlue=0;

        double denominator=0;

        for (int i=0; i < _Width; i++)
        {
            for (int j=0; j < _Height; j++)
            {
                double u = pow(TypicalityFunction(k,i,j),_DegreeTF);

                numeratorX+=u*_Pixels[i][j].X;
                numeratorY+=u*_Pixels[i][j].Y;
                numeratorRed+=u*_Pixels[i][j].Red;
                numeratorGreen+=u*_Pixels[i][j].Green;
                numeratorBlue+=u*_Pixels[i][j].Blue;

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

void PCM::PixelClustering()
{
    for (int i=0; i<_Width; i++)
    {
        for (int j=0; j<_Height; j++)
        {
            double maxTF = TypicalityFunction(0,i,j);
            _PixelLabels[i][j] = 0;

            for (int k=1; k<_ClusterCount; k++)
            {
                double currentTF = TypicalityFunction(k,i,j);
                if (currentTF > maxTF)
                {
                    maxTF = currentTF;
                    _PixelLabels[i][j] = k;
                }
            }
        }
    }
}


double PCM::TypicalityFunction(int parClusterIndex, int parColumnIndex, int parRowIndex)
{
    return 1/(1+pow(Distance(parClusterIndex,parColumnIndex,parRowIndex)/_BandWidth[parClusterIndex],2/(_DegreeTF-1)));
}
