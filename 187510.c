cmsUInt16Number CMSEXPORT cmsEvalToneCurve16(const cmsToneCurve* Curve, cmsUInt16Number v)
{
    cmsUInt16Number out;

    _cmsAssert(Curve != NULL);

    Curve ->InterpParams ->Interpolation.Lerp16(&v, &out, Curve ->InterpParams);
    return out;
}