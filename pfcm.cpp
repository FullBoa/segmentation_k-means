#include "pfcm.h"

#include "math.h"
#include "fcm.h"

#include "QDebug"

PFCM::PFCM(int parClusterCount,
           PixelRgb **parPixels,
           int parWidth,
           int parHeight,
           double parDegreeMF,
           double parDegreeTF,
           int parMaxIterationCount,
           double parPrecision)
    :KMeans(parClusterCount,
            parPixels,
            parWidth,
            parHeight,
            parMaxIterationCount,
            parPrecision),
      FCM(parClusterCount,
          parPixels,
          parWidth,
          parHeight,
          parDegreeMF,
          parMaxIterationCount,
          parPrecision),
      PCM(parClusterCount,
          parPixels,
          parWidth,
          parHeight,
          parDegreeTF,
          parMaxIterationCount,
          parPrecision)
{
}

void PFCM::Init()
{
    //Инициализация массива номеров кластеров для каждого пикселя
    _PixelLabels = new int*[_Width];
    for (int i=0; i < _Width; i++)
    {
        _PixelLabels[i] = new int[_Height];
    }

    FCM fcm(_ClusterCount,
            _Pixels,
            _Width,
            _Height,
            _DegreeMF,
            _MaxIterationCount,
            _Precision);

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
                double mf = pow(fcm.MembershipFunction(k,i,j),_DegreeMF);

                numerator+=mf*pow(fcm.Distance(k,i,j),2);
                denominator+=mf;
            }
        }

        _BandWidth[k]=sqrt(numerator/denominator);
    }
}

/*double PFCM::MembershipFunction(int parClusterIndex, int parColumnIndex, int parRowIndex)
{
    double sum = 0;
    double distCurrentCluster = Distance(parClusterIndex, parColumnIndex, parRowIndex);
    if (distCurrentCluster == 0)
    {
        return 1;
    }

    for (int i = 0; i < _ClusterCount; i++)
    {
        double distOtherCluster = Distance(i, parColumnIndex, parRowIndex);
        if (distOtherCluster == 0)
        {
            return 0;
        }
        else
        {
            sum+=pow(distCurrentCluster
                     /distOtherCluster,
                     2.0/(_DegreeMF-1));
        }
    }

    return 1.0/sum;
}*/

PixelRgb *PFCM::NewCenterPositions()
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
                double sf = pow(MembershipFunction(k,i,j),_DegreeMF)+pow(TypicalityFunction(k,i,j),_DegreeTF);

                numeratorX+=sf*_Pixels[i][j].X;
                numeratorY+=sf*_Pixels[i][j].Y;
                numeratorRed+=sf*_Pixels[i][j].Red;
                numeratorGreen+=sf*_Pixels[i][j].Green;
                numeratorBlue+=sf*_Pixels[i][j].Blue;

                denominator+=sf;
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

double PFCM::ObjectiveFunction()
{
    double sum = 0;

    for (int k=0; k<_ClusterCount; k++)
    {
        for (int i=0; i<_Width; i++)
        {
            for (int j=0; j<_Height; j++)
            {
                sum+=(pow(MembershipFunction(k,i,j),_DegreeMF)
                      +pow(TypicalityFunction(k,i,j), _DegreeTF))
                        *pow(Distance(k,i,j),2);
            }
        }
    }

    return sum;
}

void PFCM::PixelClustering()
{
    for (int i=0; i<_Width; i++)
    {
        for (int j=0; j<_Height; j++)
        {
            double maxMF = MembershipFunction(0,i,j);
            _PixelLabels[i][j] = 0;

            for (int k=1; k<_ClusterCount; k++)
            {
                double currentMF = MembershipFunction(k,i,j);
                if (currentMF > maxMF)
                {
                    maxMF = currentMF;
                    _PixelLabels[i][j] = k;
                }
            }
        }
    }
}

/*double PFCM::TypicalityFunction(int parClusterIndex, int parColumnIndex, int parRowIndex)
{
    return 1/(1+pow(Distance(parClusterIndex,parColumnIndex,parRowIndex)/_BandWidth[parClusterIndex],2/(_DegreeTF-1)));
}*/
