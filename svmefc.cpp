#include "svmefc.h"

#include "QList"
#include "QDebug"

SVMeFC::SVMeFC()
{
}

int** SVMeFC::Segmentation(PixelRgb **parPixels,
                           int parWidth,
                           int parHeight,
                           int** parPixelLabels,
                           int parClusterCount,
                           PixelRgb *parClusterCenters,
                           svm_parameter parParameters)
{
    //Разделение выборки на обучающую и тестовую
    QList<double>* distances= new QList<double>[parClusterCount]();

    //Получаем расстояния от пикселей до соответсвующих центров кластеров
    for (int i=0; i<parWidth; i++)
    {
        for (int j=0; j<parHeight; j++)
        {
            distances[parPixelLabels[i][j]].append(KMeans::Distance(parPixels[i][j],
                                                                    parClusterCenters[parPixelLabels[i][j]]));
        }
    }

    for (int k=0; k<parClusterCount; k++)
    {
        qSort(distances[k]);
    }

    double* averageDistants = new double[parClusterCount];
    for (int k=0; k<parClusterCount; k++)
    {
        qDebug() << "in cluster " << k;
        qDebug() << "min distance: " << distances[k].at(0);
        qDebug() << "max distance: " << distances[k].at(distances[k].size()-1);
        averageDistants[k] = distances[k].at((distances[k].size()-1)/2);
        qDebug() << "average distance: " << averageDistants[k];
    }

    int** SVMLabels = new int*[parWidth];
    for (int i=0; i < parWidth; i++)
    {
        SVMLabels[i] = new int[parHeight];
    }

    QList<svm_node*> trainList;
    QList<int> labelList;

    int trainSetSize = 0;

    for (int i=0; i<parWidth; i++)
    {
        for (int j=0; j<parHeight; j++)
        {
            //Точка расположена близко к центру кластера
            if (KMeans::Distance(parPixels[i][j],parClusterCenters[parPixelLabels[i][j]])
                    <= averageDistants[parPixelLabels[i][j]])
            {
                SVMLabels[i][j] = parPixelLabels[i][j];

                svm_node* node = new svm_node[6]; //5 под размерность и 1 под (индекс -1)
                node[0].index = 1;
                node[0].value =parPixels[i][j].X;

                node[1].index = 2;
                node[1].value =parPixels[i][j].Y;

                node[2].index=3;
                node[2].value=parPixels[i][j].Red;

                node[3].index=4;
                node[3].value=parPixels[i][j].Green;

                node[4].index=5;
                node[4].value=parPixels[i][j].Blue;

                node[5].index=-1;

                trainList.append(node);
                labelList.append(parPixelLabels[i][j]);

                trainSetSize++;
            }
            //Точка расположена далеко от центра кластера
            else
            {
                SVMLabels[i][j] = -1;
            }
        }
    }

    int testSetSize = parWidth*parHeight - trainSetSize;

    svm_problem trainProblem;
    trainProblem.l = trainSetSize;
    trainProblem.y = new double[trainSetSize];
    trainProblem.x = new svm_node*[trainSetSize];

    for(int i=0; i<trainSetSize; i++)
    {
        trainProblem.y[i]=labelList.at(i);
        trainProblem.x[i]=trainList.at(i);
    }

    qDebug() << "train set size: " << trainProblem.l;
    qDebug() << "test set size: " << testSetSize;

    if (svm_check_parameter(&trainProblem, &parParameters) == NULL)
    {
        svm_model* model=svm_train(&trainProblem,&parParameters);

        for (int i=0; i<parWidth; i++)
        {
            for (int j=0; j<parHeight; j++)
            {
                if (SVMLabels[i][j] == -1)
                {
                    svm_node* node = new svm_node[6]; //5 под размерность и 1 под (индекс -1)
                    node[0].index = 1;
                    node[0].value =parPixels[i][j].X;

                    node[1].index = 2;
                    node[1].value =parPixels[i][j].Y;

                    node[2].index=3;
                    node[2].value=parPixels[i][j].Red;

                    node[3].index=4;
                    node[3].value=parPixels[i][j].Green;

                    node[4].index=5;
                    node[4].value=parPixels[i][j].Blue;

                    node[5].index=-1;

                    SVMLabels[i][j] = (int)svm_predict(model,node);

                    delete node;
                }
            }
        }
    }

    return SVMLabels;
}


int** SVMeFC::SegmentationCSPA(PixelRgb **parPixels,
                               int parWidth,
                               int parHeight,
                               int ***parPixelLabels,
                               int parClusterCount,
                               PixelRgb **parClusterCenters,
                               int parMethodCount,
                               svm_parameter parParameters)
{
    int*** svmLabels = new int**[parMethodCount];
    for (int i=0; i<parMethodCount; i++)
    {
        svmLabels[i] = Segmentation(parPixels,
                                    parWidth,
                                    parHeight,
                                    parPixelLabels[i],
                                    parClusterCount,
                                    parClusterCenters[i],
                                    parParameters);
    }

    int** resultLabels = new int*[parWidth];
    for (int i=0; i<parWidth; i++)
    {
        resultLabels[i] = new int[parHeight];
    }

    for (int i=0; i<parWidth; i++)
    {
        for (int j=0; j<parHeight; j++)
        {
            int* voteCount = new int[parClusterCount];
            for (int k=0; k<parClusterCount; k++)
            {
                voteCount[k] = 0;
            }

            for (int k=0; k<parMethodCount; k++)
            {
                //Для k-ого метода метка пикселя с индексами i,j увеличиваем значение голосов в выбранном кластере
                voteCount[svmLabels[k][i][j]]++;
            }

            int maxIndex = 0;
            int maxVotes = voteCount[0];

            for (int k=0; k<parClusterCount; k++)
            {
                if (voteCount[k] > maxVotes)
                {
                    maxIndex = k;
                    maxVotes = voteCount[k];
                }
            }

            resultLabels[i][j] = maxIndex;
        }
    }

    return resultLabels;
}
