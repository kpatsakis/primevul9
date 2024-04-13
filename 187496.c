cmsFloat64Number CMSEXPORT cmsEstimateGamma(const cmsToneCurve* t, cmsFloat64Number Precision)
{
    cmsFloat64Number gamma, sum, sum2;
    cmsFloat64Number n, x, y, Std;
    cmsUInt32Number i;

    _cmsAssert(t != NULL);

    sum = sum2 = n = 0;

    // Excluding endpoints
    for (i=1; i < (MAX_NODES_IN_CURVE-1); i++) {

        x = (cmsFloat64Number) i / (MAX_NODES_IN_CURVE-1);
        y = (cmsFloat64Number) cmsEvalToneCurveFloat(t, (cmsFloat32Number) x);

        // Avoid 7% on lower part to prevent
        // artifacts due to linear ramps

        if (y > 0. && y < 1. && x > 0.07) {

            gamma = log(y) / log(x);
            sum  += gamma;
            sum2 += gamma * gamma;
            n++;
        }
    }

    // Take a look on SD to see if gamma isn't exponential at all
    Std = sqrt((n * sum2 - sum * sum) / (n*(n-1)));

    if (Std > Precision)
        return -1.0;

    return (sum / n);   // The mean
}