#include "fcm.h"

#include "math.h"

#include "QDebug"

FCM::FCM(int parClusterCount, QImage parImage, double parEpsilon, double parM) : KMeans (parClusterCount,parImage)
{
    _Epsilon = parEpsilon;

    if (parM > 1)
    {
        _M = parM;
    }
    else
    {
        _M = 2;
    }

    _IsClustered = false;
}

int **FCM::Clustering(int parMaxIterationCount)
{
    //Инициализация центроидов и массива принадлежности пикселей сегментам
    Init();

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
        qDebug() << "fcm iteration " << iterationCount;
        qDebug() << "Objective FunctionValue = " << newObjectiveFunctionValue;
        qDebug() << fabs((objectiveFunctionValue - newObjectiveFunctionValue)/newObjectiveFunctionValue);

        if (fabs((objectiveFunctionValue - newObjectiveFunctionValue)/newObjectiveFunctionValue > _Epsilon))
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

ClusterCenterRgb* FCM::GetClusterCenters(int parMaxIterationCount)
{
    //Инициализация центроидов и массива принадлежности пикселей сегментам
    Init();

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
      //  qDebug() << "Objective FunctionValue = " << newObjectiveFunctionValue;
      //  qDebug() << fabs((objectiveFunctionValue - newObjectiveFunctionValue)/newObjectiveFunctionValue);

        if (fabs((objectiveFunctionValue - newObjectiveFunctionValue)/newObjectiveFunctionValue > _Epsilon))
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

    return _ClusterCenters;
}



void FCM::Init()
{
    //Инициализация массива центроидов
    _ClusterCenters = new ClusterCenterRgb[_ClusterCount];
    for (int i = 0; i < _ClusterCount; i++)
    {
        //переменная для получения целой части числа
        double* intPart = new double;
        *intPart = 0;

        modf(qrand() / (double) INT_MAX * _Image.width(), intPart);
        _ClusterCenters[i].X = (int) *intPart;

        modf(qrand() / (double) INT_MAX * _Image.height(), intPart);
        _ClusterCenters[i].Y = (int) *intPart;\

        QRgb pixel = _Image.pixel(_ClusterCenters[i].X, _ClusterCenters[i].Y);
        _ClusterCenters[i].Red = qRed(pixel);
        _ClusterCenters[i].Green = qGreen(pixel);
        _ClusterCenters[i].Blue = qBlue(pixel);
    }
}

double FCM::MembershipFunction(int parClusterIndex, int parPixelIndexI, int parPixelIndexJ)
{
    double sum = 0;
    for (int i = 0; i < _ClusterCount; i++)
    {
        double distCurrentCluster = Distance(parClusterIndex, parPixelIndexI, parPixelIndexJ);
        double distOtherCluster = Distance(i, parPixelIndexI, parPixelIndexJ);
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
                     2.0/(_M-1));
        }
    }

    return 1.0/sum;
}

ClusterCenterRgb* FCM::NewCenterPositions()
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
                double u = pow(MembershipFunction(k,i,j),_M);

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

double FCM::ObjectiveFunction()
{
    double sum=0;
    for (int i=0; i<_Image.width(); i++)
    {
        for (int j=0; j<_Image.height(); j++)
        {
            for (int k=0; k<_ClusterCount; k++)
            {
                sum+=pow(MembershipFunction(k,i,j),_M)*pow(Distance(k,i,j),2);
            }
        }
    }

    return sum;
}
