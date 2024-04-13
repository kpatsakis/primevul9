void CMSEXPORT cmsFreeToneCurveTriple(cmsToneCurve* Curve[3])
{

    _cmsAssert(Curve != NULL);

    if (Curve[0] != NULL) cmsFreeToneCurve(Curve[0]);
    if (Curve[1] != NULL) cmsFreeToneCurve(Curve[1]);
    if (Curve[2] != NULL) cmsFreeToneCurve(Curve[2]);

    Curve[0] = Curve[1] = Curve[2] = NULL;
}