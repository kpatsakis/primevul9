cmsBool CMSEXPORT cmsIsToneCurveLinear(const cmsToneCurve* Curve)
{
    cmsUInt32Number i;
    int diff;

    _cmsAssert(Curve != NULL);

    for (i=0; i < Curve ->nEntries; i++) {

        diff = abs((int) Curve->Table16[i] - (int) _cmsQuantizeVal(i, Curve ->nEntries));
        if (diff > 0x0f)
            return FALSE;
    }

    return TRUE;
}