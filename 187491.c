cmsBool  CMSEXPORT cmsIsToneCurveDescending(const cmsToneCurve* t)
{
    _cmsAssert(t != NULL);

    return t ->Table16[0] > t ->Table16[t ->nEntries-1];
}