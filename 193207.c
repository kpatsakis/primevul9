OFCondition DcmSCP::sendACTIONResponse(const T_ASC_PresentationContextID presID,
                                       const Uint16 messageID,
                                       const OFString &sopClassUID,
                                       const OFString &sopInstanceUID,
                                       const Uint16 rspStatusCode)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_N_ActionRSP &actionRsp = response.msg.NActionRSP;
  response.CommandField = DIMSE_N_ACTION_RSP;
  actionRsp.MessageIDBeingRespondedTo = messageID;
  actionRsp.DimseStatus = rspStatusCode;
  actionRsp.DataSetType = DIMSE_DATASET_NULL;
  // Always send the optional fields "Affected SOP Class UID" and "Affected SOP Instance UID"
  actionRsp.opts = O_NACTION_AFFECTEDSOPCLASSUID | O_NACTION_AFFECTEDSOPINSTANCEUID;
  OFStandard::strlcpy(actionRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(actionRsp.AffectedSOPClassUID));
  OFStandard::strlcpy(actionRsp.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(actionRsp.AffectedSOPInstanceUID));
  // Do not send any other optional fields, e.g. "Action Type ID"

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-ACTION Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending N-ACTION Response (" << DU_nactionStatusString(rspStatusCode) << ")");
  }

  // Send response message
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-ACTION response: " << DimseCondition::dump(tempStr, cond));
  }

  return cond;
}