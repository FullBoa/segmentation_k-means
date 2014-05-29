#include "fcm.h"

#include "math.h"

#include "QDebug"

FCM::FCM(int parClusterCount,
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
        _DegreeMF = parDegree;
    }
    else
    {
        _DegreeMF = DEFAULT_DEGREE;
    }
}

double FCM::MembershipFunction(int parClusterIndex, int parColumnIndex, int parRowIndex)
{
    double sum = 0;
    for (int i = 0; i < _ClusterCount; i++)
    {
        double distCurrentCluster = Distance(parClusterIndex, parColumnIndex, parRowIndex);
        double distOtherCluster = Distance(i, parColumnIndex, parRowIndex);
        if (distCurrentCluster == 0)
        {
            return 1;
        }
        else if (distOtherCluster == 0)
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
}

PixelRgb *FCM::NewCenterPositions()
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
                double u = pow(MembershipFunction(k,i,j),_DegreeMF);

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

double FCM::ObjectiveFunction()
{
    double sum=0;
    for (int i=0; i<_Width; i++)
    {
        for (int j=0; j<_Height; j++)
        {
            for (int k=0; k<_ClusterCount; k++)
            {
                sum+=pow(MembershipFunction(k,i,j),_DegreeMF)*pow(Distance(k,i,j),2);
            }
        }
    }

    return sum;
}

void FCM::PixelClustering()
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
