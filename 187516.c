cmsToneCurve* CMSEXPORT cmsBuildSegmentedToneCurve(cmsContext ContextID,
                                                   cmsInt32Number nSegments, const cmsCurveSegment Segments[])
{
    int i;
    cmsFloat64Number R, Val;
    cmsToneCurve* g;
    int nGridPoints = 4096;

    _cmsAssert(Segments != NULL);

    // Optimizatin for identity curves.
    if (nSegments == 1 && Segments[0].Type == 1) {

        nGridPoints = EntriesByGamma(Segments[0].Params[0]);
    }

    g = AllocateToneCurveStruct(ContextID, nGridPoints, nSegments, Segments, NULL);
    if (g == NULL) return NULL;

    // Once we have the floating point version, we can approximate a 16 bit table of 4096 entries
    // for performance reasons. This table would normally not be used except on 8/16 bits transforms.
    for (i=0; i < nGridPoints; i++) {

        R   = (cmsFloat64Number) i / (nGridPoints-1);

        Val = EvalSegmentedFn(g, R);

        // Round and saturate
        g ->Table16[i] = _cmsQuickSaturateWord(Val * 65535.0);
    }

    return g;
}