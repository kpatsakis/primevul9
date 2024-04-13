cmsInt32Number  CMSEXPORT cmsGetToneCurveParametricType(const cmsToneCurve* t)
{
    _cmsAssert(t != NULL);

    if (t -> nSegments != 1) return 0;
    return t ->Segments[0].Type;
}