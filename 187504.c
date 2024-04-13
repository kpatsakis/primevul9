cmsToneCurve* CMSEXPORT cmsJoinToneCurve(cmsContext ContextID,
                                      const cmsToneCurve* X,
                                      const cmsToneCurve* Y, cmsUInt32Number nResultingPoints)
{
    cmsToneCurve* out = NULL;
    cmsToneCurve* Yreversed = NULL;
    cmsFloat32Number t, x;
    cmsFloat32Number* Res = NULL;
    cmsUInt32Number i;


    _cmsAssert(X != NULL);
    _cmsAssert(Y != NULL);

    Yreversed = cmsReverseToneCurveEx(nResultingPoints, Y);
    if (Yreversed == NULL) goto Error;

    Res = (cmsFloat32Number*) _cmsCalloc(ContextID, nResultingPoints, sizeof(cmsFloat32Number));
    if (Res == NULL) goto Error;

    //Iterate
    for (i=0; i <  nResultingPoints; i++) {

        t = (cmsFloat32Number) i / (nResultingPoints-1);
        x = cmsEvalToneCurveFloat(X,  t);
        Res[i] = cmsEvalToneCurveFloat(Yreversed, x);
    }

    // Allocate space for output
    out = cmsBuildTabulatedToneCurveFloat(ContextID, nResultingPoints, Res);

Error:

    if (Res != NULL) _cmsFree(ContextID, Res);
    if (Yreversed != NULL) cmsFreeToneCurve(Yreversed);

    return out;
}