cmsBool  CMSEXPORT cmsSmoothToneCurve(cmsToneCurve* Tab, cmsFloat64Number lambda)
{
    cmsFloat32Number w[MAX_NODES_IN_CURVE], y[MAX_NODES_IN_CURVE], z[MAX_NODES_IN_CURVE];
    int i, nItems, Zeros, Poles;

    if (Tab == NULL) return FALSE;

    if (cmsIsToneCurveLinear(Tab)) return TRUE; // Nothing to do

    nItems = Tab -> nEntries;

    if (nItems >= MAX_NODES_IN_CURVE) {
        cmsSignalError(Tab ->InterpParams->ContextID, cmsERROR_RANGE, "cmsSmoothToneCurve: too many points.");
        return FALSE;
    }

    memset(w, 0, nItems * sizeof(cmsFloat32Number));
    memset(y, 0, nItems * sizeof(cmsFloat32Number));
    memset(z, 0, nItems * sizeof(cmsFloat32Number));

    for (i=0; i < nItems; i++)
    {
        y[i+1] = (cmsFloat32Number) Tab -> Table16[i];
        w[i+1] = 1.0;
    }

    if (!smooth2(Tab ->InterpParams->ContextID, w, y, z, (cmsFloat32Number) lambda, nItems)) return FALSE;

    // Do some reality - checking...
    Zeros = Poles = 0;
    for (i=nItems; i > 1; --i) {

        if (z[i] == 0.) Zeros++;
        if (z[i] >= 65535.) Poles++;
        if (z[i] < z[i-1]) {
            cmsSignalError(Tab ->InterpParams->ContextID, cmsERROR_RANGE, "cmsSmoothToneCurve: Non-Monotonic.");
            return FALSE;
        }
    }

    if (Zeros > (nItems / 3)) {
        cmsSignalError(Tab ->InterpParams->ContextID, cmsERROR_RANGE, "cmsSmoothToneCurve: Degenerated, mostly zeros.");
        return FALSE;
    }
    if (Poles > (nItems / 3)) {
        cmsSignalError(Tab ->InterpParams->ContextID, cmsERROR_RANGE, "cmsSmoothToneCurve: Degenerated, mostly poles.");
        return FALSE;
    }

    // Seems ok
    for (i=0; i < nItems; i++) {

        // Clamp to cmsUInt16Number
        Tab -> Table16[i] = _cmsQuickSaturateWord(z[i+1]);
    }

    return TRUE;
}