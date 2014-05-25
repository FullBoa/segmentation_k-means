#include "pfcm.h"

#include "math.h"
#include "fcm.h"

#include "QDebug"

PFCM::PFCM(int parClusterCount, QImage parImage, double parEpsilon, double parMu, double parMw)
    :KMeans(parClusterCount, parImage)
{
    if (parMu > 1)
    {
        _Mu = parMu;
    }
    else
    {
        _Mu = 2;
    }

    if (parMw > 1)
    {
        _Mw= parMw;
    }
    else
    {
        _Mw = 2;
    }

    _Epsilon = parEpsilon;
}

int** PFCM::Clustering(int parMaxIterationCount)
{
    //Инициализация центроидов и массива принадлежности пикселей сегментам
    Init(parMaxIterationCount);

    //флаг окончания сегментации
    bool done = false;

    //Количество пройденных итераций
    int iterationCount = 0;

    double objectiveFunctionValue = ObjectiveFunction();
    do
    {
        iterationCount++;

        //Вычисление новых центров масс кластеров.
        _ClusterCenters = NewCenterPositions();

        double newObjectiveFunctionValue = ObjectiveFunction();
        qDebug() << "pfcm iteration " << iterationCount;
        qDebug() << "Objective Function Value = " << newObjectiveFunctionValue;
        qDebug() << fabs(objectiveFunctionValue - newObjectiveFunctionValue)/newObjectiveFunctionValue;

        if (fabs(objectiveFunctionValue - newObjectiveFunctionValue)/newObjectiveFunctionValue > _Epsilon)
        {
            objectiveFunctionValue = newObjectiveFunctionValue;
        }
        else
        {
            done = true;
        }
    }
    while(!done && (iterationCount < parMaxIterationCount));

    _LastIterationCount = iterationCount;

    _IsClustered = true;

    int** pixels = new int*[_Image.width()];
    for (int i=0; i < _Image.width(); i++)
    {
        pixels[i] = new int[_Image.height()];
    }

    for (int i=0; i<_Image.width(); i++)
    {
        for (int j=0; j<_Image.height(); j++)
        {
            double maxMF = MembershipFunction(0,i,j);
            pixels[i][j] = 0;

            for (int k=1; k<_ClusterCount; k++)
            {
                double currentMF = MembershipFunction(k,i,j);
                if (currentMF > maxMF)
                {
                    maxMF = currentMF;
                    pixels[i][j] = k;
                }
            }
        }
    }

    return pixels;
}

void PFCM::Init(int parMaxIterationCount)
{
    FCM fcm(_ClusterCount,_Image,_Epsilon,_Mu);

    if (_ClusterCenters == NULL)
    {
        _ClusterCenters = fcm.GetClusterCenters(parMaxIterationCount);
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

        for (int i=0; i<_Image.width(); i++)
        {
            for (int j=0; j<_Image.height(); j++)
            {
                double mf = pow(fcm.MembershipFunction(k,i,j),_Mu);

                numerator+=mf*pow(fcm.Distance(k,i,j),2);
                denominator+=mf;
            }
        }

        _BandWidth[k]=sqrt(numerator/denominator);
    }
}

double PFCM::MembershipFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ)
{
    double sum = 0;
    double distCurrentCluster = Distance(parClusterIndex, parPixelIndexI, parPixelIndexJ);
    if (distCurrentCluster == 0)
    {
        return 1;
    }

    for (int i = 0; i < _ClusterCount; i++)
    {
        double distOtherCluster = Distance(i, parPixelIndexI, parPixelIndexJ);
        if (distOtherCluster == 0)
        {
            return 0;
        }
        else
        {
            sum+=pow(distCurrentCluster
                     /distOtherCluster,
                     2.0/(_Mu-1));
        }
    }

    return 1.0/sum;
}

ClusterCenterRgb* PFCM::NewCenterPositions()
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
                double sf = pow(MembershipFunction(k,i,j),_Mu)+pow(TypicalityFunction(k,i,j),_Mw);

                numeratorX+=sf*i;
                numeratorY+=sf*j;
                numeratorRed+=sf*qRed(_Image.pixel(i,j));
                numeratorGreen+=sf*qGreen(_Image.pixel(i,j));
                numeratorBlue+=sf*qBlue(_Image.pixel(i,j));

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
        for (int i=0; i<_Image.width(); i++)
        {
            for (int j=0; j<_Image.height(); j++)
            {
                sum+=(pow(MembershipFunction(k,i,j),_Mu)
                      +pow(TypicalityFunction(k,i,j), _Mw))
                        *pow(Distance(k,i,j),2);
            }
        }
    }

    return sum;
}

double PFCM::TypicalityFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ)
{
    return 1/(1+pow(Distance(parClusterIndex,parPixelIndexI,parPixelIndexJ)/_BandWidth[parClusterIndex],2/(_Mw-1)));
}
