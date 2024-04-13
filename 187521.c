cmsUInt32Number CMSEXPORT cmsGetToneCurveEstimatedTableEntries(const cmsToneCurve* t)
{
    _cmsAssert(t != NULL);
    return t ->nEntries;
}