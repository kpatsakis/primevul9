OFCondition DcmSCP::sendFINDResponse(const T_ASC_PresentationContextID presID,
                                     const Uint16 messageID,
                                     const OFString &sopClassUID,
                                     DcmDataset *rspDataset,
                                     const Uint16 rspStatusCode,
                                     DcmDataset* statusDetail)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_C_FindRSP &findRsp = response.msg.CFindRSP;
  response.CommandField = DIMSE_C_FIND_RSP;
  findRsp.MessageIDBeingRespondedTo = messageID;
  findRsp.DimseStatus = rspStatusCode;
  // Always send (the optional) field "Affected SOP Class UID"
  findRsp.opts = O_FIND_AFFECTEDSOPCLASSUID;
  OFStandard::strlcpy(findRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(findRsp.AffectedSOPClassUID));

  if (rspDataset)
    findRsp.DataSetType = DIMSE_DATASET_PRESENT;
  else
    findRsp.DataSetType = DIMSE_DATASET_NULL;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-FIND Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, rspDataset, presID));
  } else {
    DCMNET_INFO("Sending C-FIND Response (" << DU_cfindStatusString(rspStatusCode) << ")");
  }

  // Send response message with dataset
  cond = sendDIMSEMessage(presID, &response, rspDataset /* dataObject */, statusDetail);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-FIND response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  return cond;
}