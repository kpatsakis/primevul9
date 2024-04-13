cmsToneCurve* CMSEXPORT cmsReverseToneCurveEx(cmsInt32Number nResultSamples, const cmsToneCurve* InCurve)
{
    cmsToneCurve *out;
    cmsFloat64Number a = 0, b = 0, y, x1, y1, x2, y2;
    int i, j;
    int Ascending;

    _cmsAssert(InCurve != NULL);

    // Try to reverse it analytically whatever possible
    if (InCurve ->nSegments == 1 && InCurve ->Segments[0].Type > 0 && InCurve -> Segments[0].Type <= 5) {

        return cmsBuildParametricToneCurve(InCurve ->InterpParams->ContextID,
                                       -(InCurve -> Segments[0].Type),
                                       InCurve -> Segments[0].Params);
    }

    // Nope, reverse the table.
    out = cmsBuildTabulatedToneCurve16(InCurve ->InterpParams->ContextID, nResultSamples, NULL);
    if (out == NULL)
        return NULL;

    // We want to know if this is an ascending or descending table
    Ascending = !cmsIsToneCurveDescending(InCurve);

    // Iterate across Y axis
    for (i=0; i <  nResultSamples; i++) {

        y = (cmsFloat64Number) i * 65535.0 / (nResultSamples - 1);

        // Find interval in which y is within.
        j = GetInterval(y, InCurve->Table16, InCurve->InterpParams);
        if (j >= 0) {


            // Get limits of interval
            x1 = InCurve ->Table16[j];
            x2 = InCurve ->Table16[j+1];

            y1 = (cmsFloat64Number) (j * 65535.0) / (InCurve ->nEntries - 1);
            y2 = (cmsFloat64Number) ((j+1) * 65535.0 ) / (InCurve ->nEntries - 1);

            // If collapsed, then use any
            if (x1 == x2) {

                out ->Table16[i] = _cmsQuickSaturateWord(Ascending ? y2 : y1);
                continue;

            } else {

                // Interpolate
                a = (y2 - y1) / (x2 - x1);
                b = y2 - a * x2;
            }
        }

        out ->Table16[i] = _cmsQuickSaturateWord(a* y + b);
    }


    return out;
}