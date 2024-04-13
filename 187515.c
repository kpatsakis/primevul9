cmsToneCurve* CMSEXPORT cmsBuildParametricToneCurve(cmsContext ContextID, cmsInt32Number Type, const cmsFloat64Number Params[])
{
    cmsCurveSegment Seg0;
    int Pos = 0;
    cmsUInt32Number size;
    _cmsParametricCurvesCollection* c = GetParametricCurveByType(Type, &Pos);

    _cmsAssert(Params != NULL);

    if (c == NULL) {
         cmsSignalError(ContextID, cmsERROR_UNKNOWN_EXTENSION, "Invalid parametric curve type %d", Type);
        return NULL;
    }

    memset(&Seg0, 0, sizeof(Seg0));

    Seg0.x0   = MINUS_INF;
    Seg0.x1   = PLUS_INF;
    Seg0.Type = Type;

    size = c->ParameterCount[Pos] * sizeof(cmsFloat64Number);
    memmove(Seg0.Params, Params, size);

    return cmsBuildSegmentedToneCurve(ContextID, 1, &Seg0);
}