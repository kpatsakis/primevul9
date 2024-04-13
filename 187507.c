const cmsUInt16Number* CMSEXPORT cmsGetToneCurveEstimatedTable(const cmsToneCurve* t)
{
    _cmsAssert(t != NULL);
    return t ->Table16;
}