cmsToneCurve* CMSEXPORT cmsReverseToneCurve(const cmsToneCurve* InGamma)
{
    _cmsAssert(InGamma != NULL);

    return cmsReverseToneCurveEx(4096, InGamma);
}