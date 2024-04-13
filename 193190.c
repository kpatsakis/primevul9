refuseAssociation(T_ASC_Association *assoc, refuseReason reason)
{
    T_ASC_RejectParameters rej;

    switch (reason)
    {
    case ref_TooManyAssociations:
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
        rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
        break;
    case ref_CannotFork:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
        rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
        break;
    case ref_BadAppContext:
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
        break;
    case ref_BadAEPeer:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
        break;
    case ref_BadAEService:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
        break;
    case ref_NoReason:
    default:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_NOREASON;
        break;
    }

    OFCondition cond = ASC_rejectAssociation(assoc, &rej);
    errorCond(cond, "Association Reject Failed:");
    return cond;
}