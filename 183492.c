cmsStage* CMSEXPORT cmsStageDup(cmsStage* mpe)
{
    cmsStage* NewMPE;

    if (mpe == NULL) return NULL;
    NewMPE = _cmsStageAllocPlaceholder(mpe ->ContextID,
                                     mpe ->Type,
                                     mpe ->InputChannels,
                                     mpe ->OutputChannels,
                                     mpe ->EvalPtr,
                                     mpe ->DupElemPtr,
                                     mpe ->FreePtr,
                                     NULL);
    if (NewMPE == NULL) return NULL;

    NewMPE ->Implements = mpe ->Implements;

    if (mpe ->DupElemPtr) {

        NewMPE ->Data = mpe ->DupElemPtr(mpe);

        if (NewMPE->Data == NULL) {

            cmsStageFree(NewMPE);
            return NULL;
        }

    } else {

        NewMPE ->Data       = NULL;
    }

    return NewMPE;
}