moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request,
    int responseCount, T_DIMSE_C_MoveRSP *response)
{
    OFCondition cond = EC_Normal;
    MyCallbackInfo *myCallbackData;
    OFString temp_str;

    myCallbackData = OFstatic_cast(MyCallbackInfo*, callbackData);

    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL)) {
        OFLOG_INFO(movescuLogger, "Received Move Response " << responseCount);
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, *response, DIMSE_INCOMING));
    } else {
        OFLOG_INFO(movescuLogger, "Received Move Response " << responseCount << " (" << DU_cmoveStatusString(response->DimseStatus) << ")");
    }

    /* should we send a cancel back ?? */
    if (opt_cancelAfterNResponses == responseCount) {
        OFLOG_INFO(movescuLogger, "Sending Cancel Request (MsgID " << request->MessageID
            << ", PresID " << OFstatic_cast(unsigned int, myCallbackData->presId) << ")");
        cond = DIMSE_sendCancelRequest(myCallbackData->assoc,
            myCallbackData->presId, request->MessageID);
        if (cond != EC_Normal) {
            OFLOG_ERROR(movescuLogger, "Cancel Request Failed: " << DimseCondition::dump(temp_str, cond));
        }
    }
}