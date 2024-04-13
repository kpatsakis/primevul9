void* CLUTElemDup(cmsStage* mpe)
{
    _cmsStageCLutData* Data = (_cmsStageCLutData*) mpe ->Data;
    _cmsStageCLutData* NewElem;


    NewElem = (_cmsStageCLutData*) _cmsMallocZero(mpe ->ContextID, sizeof(_cmsStageCLutData));
    if (NewElem == NULL) return NULL;

    NewElem ->nEntries       = Data ->nEntries;
    NewElem ->HasFloatValues = Data ->HasFloatValues;

    if (Data ->Tab.T) {

        if (Data ->HasFloatValues) {
            NewElem ->Tab.TFloat = (cmsFloat32Number*) _cmsDupMem(mpe ->ContextID, Data ->Tab.TFloat, Data ->nEntries * sizeof (cmsFloat32Number));
            if (NewElem ->Tab.TFloat == NULL)
                goto Error;
        } else {
            NewElem ->Tab.T = (cmsUInt16Number*) _cmsDupMem(mpe ->ContextID, Data ->Tab.T, Data ->nEntries * sizeof (cmsUInt16Number));
            if (NewElem ->Tab.TFloat == NULL)
                goto Error;
        }
    }

    NewElem ->Params   = _cmsComputeInterpParamsEx(mpe ->ContextID,
                                                   Data ->Params ->nSamples,
                                                   Data ->Params ->nInputs,
                                                   Data ->Params ->nOutputs,
                                                   NewElem ->Tab.T,
                                                   Data ->Params ->dwFlags);
    if (NewElem->Params != NULL)
        return (void*) NewElem;
 Error:
    if (NewElem->Tab.T)
        // This works for both types
        _cmsFree(mpe ->ContextID, NewElem -> Tab.T);
    _cmsFree(mpe ->ContextID, NewElem);
    return NULL;
}