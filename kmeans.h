#ifndef KMEANS_H
#define KMEANS_H

//Структура для описание цветного пикселя
struct PixelRgb
{
    double X;
    double Y;
    double Red;
    double Green;
    double Blue;
};

class KMeans
{
public:

    //Максимальное количество итерации при сегментации
    static int const DEFAULT_MAX_ITERATION_COUNT = 100;

    //Значение изменения рассояния по умолчанию, при изменении позиции цетроидов меньше которого,
    //считается, что изменений не было
    static double const DEFAULT_PRECISION = 0.001;

    //Конструктор класса
    //parClusterCount - количество кластеров для поиска
    //parPixels - пиксели сегментируемого изображения
    //parWidth - ширина изображения
    //parHeight - высота изображения
    //parMaxIterationCount - максимальное количества итераций при сегментации
    //parPrecision - точность вычисления целевой функции
    KMeans(int parClusterCount,
           PixelRgb** parPixels,
           int parWidth,
           int parHeight,
           int parMaxIterationCount = DEFAULT_MAX_ITERATION_COUNT,
           int parPrecision = DEFAULT_PRECISION);

    //Очистить значения центров кластеров
    void ClearClusterCenters();

    //Получение центров кластеров
    PixelRgb* ClusterCenters();

    //Получение количества кластеров
    int ClusterCount();

    //Сегментация изображения
    //Возвращает двумерных массив принадлежности пикселя сегментам
    int** Clustering();

    //Расчет растояния между двумя пикселями
    static double Distance(PixelRgb parFirstPixel,
                            PixelRgb parSecondPixel);

    //Расчет растояния между центром кластера с индексом parClusterIndex
    // и пикселем в позиции (parColumnIndex, parRowIndex)
    double Distance(int parClusterIndex, int parColumnIndex, int parRowIndex);

    //Найти позиции центров кластеров
    void FindClusterCenters();

    //Получение количества итераций при последний сегментации
    int LastIterationCount();

    //Получение значения максимального количества итераций
    int MaxIterationCount();

    //Расчет значения целевой функции
    double ObjectiveFunction();

    //Получение пиксели сегментируемого изображения
    PixelRgb** Pixels();

    //Получение значения точности вычисления целевой фунции
    double Precision();

    //Задание значений центров кластеров
    void SetClusterCenters(PixelRgb* parClusterCenters);

    //Задание количества сегментов, на которое нужно будет разбить изображение
    void SetClusterCount(int parClusterCount);

    //Установка значения максимального количества итераций
    void SetMaxIterationCount(int parMaxIterationCount);

    //Установка значения точности вычисления целевой функции
    void SetPrecision(double parPrecision);

protected:
    //Центры кластеров были найдены
    bool _CentersFound;

    //Центры кластеров
    PixelRgb* _ClusterCenters;

    //Количество кластеров
    uint _ClusterCount;

    //Высота изображения
    int _Height;

    //Количество итераций при последней сегментации
    int _LastIterationCount;

    //Максимальное количество итераций сегментации
    int _MaxIterationCount;

    //Получение новых позиций центроидов
    PixelRgb* NewCenterPositions();

    //Пиксели были распределены по кластерам
    bool _PixelsClustered;

    //Отнесение пикселя к сегментам
    void PixelClustering();

    //Принадлежность пикселей к кластерам
    int** _PixelLabels;

    //Пиксели сегментируемого изображения
    PixelRgb** _Pixels;

    //Точность вычисления целевой функции
    double _Precision;

    //Ширина изображения
    int _Width;

private:
    //Инициализация массивов центроидов и пикселей сегментируемого изображения
    void Init();

};
#endif // KMEANS_H
