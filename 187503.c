cmsBool  CMSEXPORT cmsIsToneCurveMonotonic(const cmsToneCurve* t)
{
    int n;
    int i, last;
    cmsBool lDescending;

    _cmsAssert(t != NULL);

    // Degenerated curves are monotonic? Ok, let's pass them
    n = t ->nEntries;
    if (n < 2) return TRUE;

    // Curve direction
    lDescending = cmsIsToneCurveDescending(t);

    if (lDescending) {

        last = t ->Table16[0];

        for (i = 1; i < n; i++) {

            if (t ->Table16[i] - last > 2) // We allow some ripple
                return FALSE;
            else
                last = t ->Table16[i];

        }
    }
    else {

        last = t ->Table16[n-1];

        for (i = n-2; i >= 0; --i) {

            if (t ->Table16[i] - last > 2)
                return FALSE;
            else
                last = t ->Table16[i];

        }
    }

    return TRUE;
}