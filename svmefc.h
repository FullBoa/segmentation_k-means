#ifndef SVMEFC_H
#define SVMEFC_H

#include "svm.h"
#include "kmeans.h"

class SVMeFC
{
public:
    SVMeFC();

    static int** Segmentation(PixelRgb** parPixels,
                              int parWidth,
                              int parHeight,
                              int **parPixelLabels,
                              int parClusterCount,
                              PixelRgb *parClusterCenters,
                              svm_parameter parParameters);

    static int** SegmentationCSPA(PixelRgb** parPixels,
                                  int parWidth,
                                  int parHeight,
                                  int ***parPixelLabels,
                                  int parClusterCount,
                                  PixelRgb **parClusterCenters,
                                  int parMethodCount,
                                  svm_parameter parParameters);

    static int** SegmentationEncemble(PixelRgb** parPixels,
                                      int parWidth,
                                      int parHeight,
                                      int ***parPixelLabels,
                                      int parMethodCount,
                                      svm_parameter parParameters);

private:
    svm_parameter _Parameters;
};

#endif // SVMEFC_H
