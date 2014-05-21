#include "pcm.h"

#include "math.h"
#include "fcm.h"

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

int** PCM::Clustering(int parMaxIterationCount)
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

    int** pixels = new int*[_Image.width()];
    for (int i=0; i < _Image.width(); i++)
    {
        pixels[i] = new int[_Image.height()];
    }

    for (int i=0; i<_Image.width(); i++)
    {
        for (int j=0; j<_Image.height(); j++)
        {
            double maxTF = TypicalityFunction(0,i,j);
            pixels[i][j] = 0;

            for (int k=1; k<_ClusterCount; k++)
            {
                double currentTF = TypicalityFunction(k,i,j);
                if (currentTF > maxTF)
                {
                    maxTF = currentTF;
                    pixels[i][j] = k;
                }
            }
        }
    }

    return pixels;
}

void PCM::Init(int parMaxIterationCount)
{
    FCM fcm(_ClusterCount,_Image,_Epsilon,_M);

    _ClusterCenters = fcm.GetClusterCenters(parMaxIterationCount);

    for (int k=0; k<_ClusterCount; k++)
    {
        double numerator=0;
        double denominator=0;

        for (int i=0; i<_Image.width(); i++)
        {
            for (int j=0; j<_Image.height(); j++)
            {
                double mf = pow(fcm.MembershipFunction(k,i,j),_M);

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
