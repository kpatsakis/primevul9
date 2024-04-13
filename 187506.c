cmsToneCurve* CMSEXPORT cmsBuildTabulatedToneCurve16(cmsContext ContextID, cmsInt32Number nEntries, const cmsUInt16Number Values[])
{
    return AllocateToneCurveStruct(ContextID, nEntries, 0, NULL, Values);
}