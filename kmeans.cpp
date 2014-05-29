#include "kmeans.h"
#include <QDebug>

#include "math.h"

//Конструктор класса
//parClusterCount - количество кластеров для поиска
//parPixels - пиксели сегментируемого изображения
//parWidth - ширина изображения
//parHeight - высота изображения
//parMaxIterationCount - максимальное количества итераций при сегментации
//parPrecision - точность вычисления целевой функции
KMeans::KMeans(int parClusterCount,
               PixelRgb **parPixels,
               int parWidth,
               int parHeight,
               int parMaxIterationCount,
               double parPrecision)
{
    if (parClusterCount > 0)
    {
        _ClusterCount = parClusterCount;
    }

    _Width = parWidth;
    _Height = parHeight;
    _MaxIterationCount = parMaxIterationCount;
    _Precision = parPrecision;

    _Pixels = parPixels;

    _PixelLabels = NULL;

    _ClusterCenters = NULL;

    _LastIterationCount = 0;
    _CentersFound = false;
    _PixelsClustered = false;
}

//Очистить значения центров кластеров
void KMeans::ClearClusterCenters()
{
    delete _ClusterCenters;

    _CentersFound = false;
    _PixelsClustered=false;
}

//Получение центров кластеров
PixelRgb *KMeans::ClusterCenters()
{
    return _ClusterCenters;
}

//Получение количества кластеров
int KMeans::ClusterCount()
{
    return _ClusterCount;
}

//Сегментация изображения
//Возвращает двумерных массив принадлежности пикселя сегментам
int **KMeans::Clustering()
{
    if (!_CentersFound)
    {
        FindClusterCenters();
    }

    if (!_PixelsClustered)
    {
        PixelClustering();
        _PixelsClustered = true;
    }

    return _PixelLabels;
}

//Расчет растояния между двумя пикселями
double KMeans::Distance(PixelRgb parFirstPixel,
                        PixelRgb parSecondPixel)
{
    return sqrt((parFirstPixel.X - parSecondPixel.X)*(parFirstPixel.X - parSecondPixel.X)
                + (parFirstPixel.Y - parSecondPixel.Y)*(parFirstPixel.Y - parSecondPixel.Y)
                + (parFirstPixel.Red - parSecondPixel.Red)*(parFirstPixel.Red - parSecondPixel.Red)
                + (parFirstPixel.Green - parSecondPixel.Green)*(parFirstPixel.Green - parSecondPixel.Green)
                + (parFirstPixel.Blue - parSecondPixel.Blue)*(parFirstPixel.Blue - parSecondPixel.Blue));
}

//Расчет растояния между центром кластера с индексом parClusterIndex
// и пикселем в позиции (parColumnIndex, parRowIndex)
double KMeans::Distance(int parClusterIndex,
                        int parColumnIndex,
                        int parRowIndex)
{
    return Distance(_ClusterCenters[parClusterIndex], _Pixels[parColumnIndex][parRowIndex]);
}

//Найти позиции центров кластеров
void KMeans::FindClusterCenters()
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
        delete _ClusterCenters;
        _ClusterCenters = NewCenterPositions();

        double newObjectiveFunctionValue = ObjectiveFunction();
       /* qDebug() << "Iteration number " << iterationCount;
        qDebug()<<"Objective function value = " << objectiveFunctionValue;
        qDebug()<<"dif: " << fabs(objectiveFunctionValue - newObjectiveFunctionValue)/objectiveFunctionValue;*/

        if ((fabs(objectiveFunctionValue - newObjectiveFunctionValue)/objectiveFunctionValue) > _Precision)
        {
            objectiveFunctionValue = newObjectiveFunctionValue;
        }
        else
        {
            done = true;
        }
    }
    while(!done && (iterationCount < _MaxIterationCount));

    _LastIterationCount = iterationCount;

    _CentersFound = true;

    _PixelsClustered = false;
}

//Инициализация массивов центроидов и пикселей сегментируемого изображения
void KMeans::Init()
{
    //Инициализация массива номеров кластеров для каждого пикселя
    _PixelLabels = new int*[_Width];
    for (int i=0; i < _Width; i++)
    {
        _PixelLabels[i] = new int[_Height];
    }

    //Инициализация массива центроидов
    if (_ClusterCenters == NULL)
    {
        _ClusterCenters = new PixelRgb[_ClusterCount];
        for (int i = 0; i < _ClusterCount; i++)
        {
            //переменная для получения целой части числа
            double* intPart = new double;
            *intPart = 0;

            int columnIndex;
            int rowIndex;

            modf(qrand() / (double) INT_MAX * _Width, intPart);
            columnIndex = (int) *intPart;

            modf(qrand() / (double) INT_MAX * _Height, intPart);
            rowIndex = (int) *intPart;

            _ClusterCenters[i].X = _Pixels[columnIndex][rowIndex].X;
            _ClusterCenters[i].Y = _Pixels[columnIndex][rowIndex].Y;
            _ClusterCenters[i].Red = _Pixels[columnIndex][rowIndex].Red;
            _ClusterCenters[i].Green = _Pixels[columnIndex][rowIndex].Green;
            _ClusterCenters[i].Blue = _Pixels[columnIndex][rowIndex].Blue;
        }
    }

    PixelClustering();
}

//Получение количества итераций при последний сегментации
int KMeans::LastIterationCount()
{
    return _LastIterationCount;
}

//Получение значения максимального количества итераций
int KMeans::MaxIterationCount()
{
    return _MaxIterationCount;
}

//Получение новых позиций центроидов
PixelRgb *KMeans::NewCenterPositions()
{
    //Распределение пикселей по кластерам
    PixelClustering();

    //Новые координаты центроида
    PixelRgb* newCenterPositioins = new PixelRgb[_ClusterCount];

    //Пикселей в кластере
    int* pixelInCluster = new int[_ClusterCount];

    //Инициализация новых центроидов
    for (int k = 0; k < _ClusterCount; k++)
    {
        pixelInCluster[k] = 0;

        newCenterPositioins[k].X = 0;
        newCenterPositioins[k].Y = 0;
        newCenterPositioins[k].Red = 0;
        newCenterPositioins[k].Green = 0;
        newCenterPositioins[k].Blue = 0;
    }


    //Расчет новых значений центроидов
    int numberCluster;
    for (int i = 0; i < _Width; i++)
    {
        for (int j = 0; j < _Height; j++)
        {
            numberCluster =_PixelLabels[i][j];
            pixelInCluster[numberCluster]++;

            newCenterPositioins[numberCluster].X += _Pixels[i][j].X;
            newCenterPositioins[numberCluster].Y += _Pixels[i][j].Y;
            newCenterPositioins[numberCluster].Red += _Pixels[i][j].Red;
            newCenterPositioins[numberCluster].Green += _Pixels[i][j].Green;
            newCenterPositioins[numberCluster].Blue += _Pixels[i][j].Blue;
        }
    }

    for (int k = 0; k < _ClusterCount; k++)
    {
        if (pixelInCluster[k] != 0)
        {
            newCenterPositioins[k].X /= (double)pixelInCluster[k];
            newCenterPositioins[k].Y /= (double)pixelInCluster[k];
            newCenterPositioins[k].Red /= (double)pixelInCluster[k];
            newCenterPositioins[k].Green /= (double)pixelInCluster[k];
            newCenterPositioins[k].Blue /= (double)pixelInCluster[k];
        }
    }

    return newCenterPositioins;
}

double KMeans::ObjectiveFunction()
{
    double sum = 0;

    for (int i=0; i<_Width; i++)
    {
        for (int j=0; j<_Height; j++)
        {
            sum+=Distance(_PixelLabels[i][j],i,j);
        }
    }

    return sum;
}

//Отнесение пикселя к сегментам
void KMeans::PixelClustering()
{
    //Минимальное расстояние до центроида
    double minDistance;

    //Номер ближайшего центроида
    int numberNearestCluster;

    //Проходим по всем пикселям
    for (int i = 0; i < _Width; i++)
    {
        for (int j = 0; j < _Height; j++)
        {
            //Текущий обрабатываемый пиксель
            PixelRgb currentPixel;
            currentPixel.X = _Pixels[i][j].X;
            currentPixel.Y = _Pixels[i][j].Y;
            currentPixel.Red = _Pixels[i][j].Red;
            currentPixel.Green = _Pixels[i][j].Green;
            currentPixel.Blue = _Pixels[i][j].Blue;

            //Ишем ближайший центроид
            minDistance = Distance(currentPixel,_ClusterCenters[0]);
            numberNearestCluster = 0;

            double currentDistance;
            for(int k = 1; k < _ClusterCount; k++)
            {
                currentDistance = Distance(currentPixel, _ClusterCenters[k]);
                if (currentDistance < minDistance)
                {
                    numberNearestCluster = k;
                    minDistance = currentDistance;
                }
            }

            //Приписываем пиксель кластеру
            _PixelLabels[i][j] = numberNearestCluster;
        }
    }

    _PixelsClustered = true;
}

//Получение пиксели сегментируемого изображения
PixelRgb** KMeans::Pixels()
{
    return _Pixels;
}

//Получение значения точности вычисления целевой фунции
double KMeans::Precision()
{
    return _Precision;
}

//Задание значений центров кластеров
void KMeans::SetClusterCenters(PixelRgb *parClusterCenters)
{
    if (parClusterCenters != 0)
    {
        _ClusterCenters = parClusterCenters;

        _CentersFound = false;
        _PixelsClustered = false;
    }
}

//Задание количества сегментов, на которое нужно будет разбить изображение
void KMeans::SetClusterCount(int parClusterCount)
{
    if (parClusterCount > 1)
    {
        _ClusterCount = parClusterCount;

        _CentersFound = false;
        _PixelsClustered = false;
    }
}

//Установка значения максимального количества итераций
void KMeans::SetMaxIterationCount(int parMaxIterationCount)
{
    if (parMaxIterationCount > 0)
    {
        _MaxIterationCount = parMaxIterationCount;

        _CentersFound = false;
        _PixelsClustered = false;
    }
}

//Установка значения точности вычисления целевой функции
void KMeans::SetPrecision(double parPrecision)
{
    if (parPrecision > 0 && parPrecision < 1)
    {
        _Precision = parPrecision;

        _CentersFound = false;
        _PixelsClustered = false;
    }
}
