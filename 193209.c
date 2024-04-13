OFCondition DcmSCP::handleECHORequest(T_DIMSE_C_EchoRQ &reqMessage,
                                      const T_ASC_PresentationContextID presID)
{
  OFCondition cond;
  OFString tempStr;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Received C-ECHO Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_INFO("Sending C-ECHO Response");
  } else {
    DCMNET_INFO("Received C-ECHO Request (MsgID " << reqMessage.MessageID << ")");
    DCMNET_INFO("Sending C-ECHO Response (" << DU_cechoStatusString(STATUS_Success) << ")");
  }

  // Send response message
  cond = DIMSE_sendEchoResponse( m_assoc, presID, &reqMessage, STATUS_Success, NULL );
  if( cond.bad() )
    DCMNET_ERROR("Cannot send C-ECHO Response: " << DimseCondition::dump(tempStr, cond));
  else
    DCMNET_DEBUG("C-ECHO Response successfully sent");

  return cond;
}