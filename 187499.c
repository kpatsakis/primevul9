void CMSEXPORT cmsFreeToneCurve(cmsToneCurve* Curve)
{
    cmsContext ContextID;

    if (Curve == NULL) return;

    ContextID = Curve ->InterpParams->ContextID;

    _cmsFreeInterpParams(Curve ->InterpParams);

    if (Curve -> Table16)
        _cmsFree(ContextID, Curve ->Table16);

    if (Curve ->Segments) {

        cmsUInt32Number i;

        for (i=0; i < Curve ->nSegments; i++) {

            if (Curve ->Segments[i].SampledPoints) {
                _cmsFree(ContextID, Curve ->Segments[i].SampledPoints);
            }

            if (Curve ->SegInterp[i] != 0)
                _cmsFreeInterpParams(Curve->SegInterp[i]);
        }

        _cmsFree(ContextID, Curve ->Segments);
        _cmsFree(ContextID, Curve ->SegInterp);
    }

    if (Curve -> Evals)
        _cmsFree(ContextID, Curve -> Evals);

    if (Curve) _cmsFree(ContextID, Curve);
}