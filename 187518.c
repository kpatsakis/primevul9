cmsBool  CMSEXPORT cmsIsToneCurveMultisegment(const cmsToneCurve* t)
{
    _cmsAssert(t != NULL);

    return t -> nSegments > 1;
}