static OFCondition echoSCP(T_ASC_Association *assoc, T_DIMSE_C_EchoRQ *req, T_ASC_PresentationContextID presId)
{
    OFLOG_INFO(dcmpsrcvLogger, "Received Echo SCP RQ: MsgID " << req->MessageID);
    OFCondition cond = DIMSE_sendEchoResponse(assoc, presId, req, STATUS_Success, NULL);
    errorCond(cond, "echoSCP: Echo Response Failed:");
    return cond;
}