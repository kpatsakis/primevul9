static OFCondition echoSCP( T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
  OFString temp_str;
  // assign the actual information of the C-Echo-RQ command to a local variable
  T_DIMSE_C_EchoRQ *req = &msg->msg.CEchoRQ;
  if (storescpLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    OFLOG_INFO(storescpLogger, "Received Echo Request");
    OFLOG_DEBUG(storescpLogger, DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presID));
  } else {
    OFLOG_INFO(storescpLogger, "Received Echo Request (MsgID " << req->MessageID << ")");
  }

  /* the echo succeeded !! */
  OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, req, STATUS_Success, NULL);
  if (cond.bad())
  {
    OFLOG_ERROR(storescpLogger, "Echo SCP Failed: " << DimseCondition::dump(temp_str, cond));
  }
  return cond;
}