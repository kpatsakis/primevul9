moveSCU(T_ASC_Association *assoc, const char *fname)
{
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_MoveRQ    req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US              msgId = assoc->nextMsgID++;
    DcmDataset          *rspIds = NULL;
    const char          *sopClass;
    DcmDataset          *statusDetail = NULL;
    MyCallbackInfo      callbackData;
    OFString            temp_str;

    DcmFileFormat dcmff;

    if (fname != NULL) {
        if (dcmff.loadFile(fname).bad()) {
            OFLOG_ERROR(movescuLogger, "bad DICOM file: " << fname << ": " << dcmff.error().text());
            return DIMSE_BADDATA;
        }
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys(dcmff.getDataset());

    sopClass = querySyntax[opt_queryModel].moveSyntax;

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) return DIMSE_NOVALIDPRESENTATIONCONTEXTID;

    callbackData.assoc = assoc;
    callbackData.presId = presId;

    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    if (opt_moveDestination == NULL) {
        /* set the destination to be me */
        ASC_getAPTitles(assoc->params, req.MoveDestination,
            NULL, NULL);
    } else {
        strcpy(req.MoveDestination, opt_moveDestination);
    }

    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        OFLOG_INFO(movescuLogger, "Sending Move Request");
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, req, DIMSE_OUTGOING, NULL, presId));
    } else {
        OFLOG_INFO(movescuLogger, "Sending Move Request (MsgID " << msgId << ")");
    }
    OFLOG_INFO(movescuLogger, "Request Identifiers:" << OFendl << DcmObject::PrintHelper(*dcmff.getDataset()));

    OFCondition cond = DIMSE_moveUser(assoc, presId, &req, dcmff.getDataset(),
        moveCallback, &callbackData, opt_blockMode, opt_dimse_timeout, net, subOpCallback,
        NULL, &rsp, &statusDetail, &rspIds, opt_ignorePendingDatasets);

    if (cond == EC_Normal) {
        if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL)) {
            OFLOG_INFO(movescuLogger, "Received Final Move Response");
            OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING));
            if (rspIds != NULL) {
                OFLOG_DEBUG(movescuLogger, "Response Identifiers:" << OFendl << DcmObject::PrintHelper(*rspIds));
            }
        } else {
            OFLOG_INFO(movescuLogger, "Received Final Move Response (" << DU_cmoveStatusString(rsp.DimseStatus) << ")");
        }
    } else {
        OFLOG_ERROR(movescuLogger, "Move Request Failed: " << DimseCondition::dump(temp_str, cond));
    }
    if (statusDetail != NULL) {
        OFLOG_DEBUG(movescuLogger, "Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
        delete statusDetail;
    }

    if (rspIds != NULL) delete rspIds;

    return cond;
}