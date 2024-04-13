void* CurveSetDup(cmsStage* mpe)
{
    _cmsStageToneCurvesData* Data = (_cmsStageToneCurvesData*) mpe ->Data;
    _cmsStageToneCurvesData* NewElem;
    cmsUInt32Number i;

    NewElem = (_cmsStageToneCurvesData*) _cmsMallocZero(mpe ->ContextID, sizeof(_cmsStageToneCurvesData));
    if (NewElem == NULL) return NULL;

    NewElem ->nCurves   = Data ->nCurves;
    NewElem ->TheCurves = (cmsToneCurve**) _cmsCalloc(mpe ->ContextID, NewElem ->nCurves, sizeof(cmsToneCurve*));

    if (NewElem ->TheCurves == NULL) goto Error;

    for (i=0; i < NewElem ->nCurves; i++) {

        // Duplicate each curve. It may fail.
        NewElem ->TheCurves[i] = cmsDupToneCurve(Data ->TheCurves[i]);
        if (NewElem ->TheCurves[i] == NULL) goto Error;


    }
    return (void*) NewElem;

Error:

    if (NewElem ->TheCurves != NULL) {
        for (i=0; i < NewElem ->nCurves; i++) {
            if (NewElem ->TheCurves[i])
                cmsFreeToneCurve(NewElem ->TheCurves[i]);
        }
    }
    _cmsFree(mpe ->ContextID, NewElem ->TheCurves);
    _cmsFree(mpe ->ContextID, NewElem);
    return NULL;
}