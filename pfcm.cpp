#include "pfcm.h"

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
