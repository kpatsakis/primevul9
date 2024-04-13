cmsToneCurve* CMSEXPORT cmsDupToneCurve(const cmsToneCurve* In)
{
    if (In == NULL) return NULL;

    return  AllocateToneCurveStruct(In ->InterpParams ->ContextID, In ->nEntries, In ->nSegments, In ->Segments, In ->Table16);
}