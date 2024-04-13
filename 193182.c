OFCondition DcmSCP::sendMOVEResponse(const T_ASC_PresentationContextID presID,
                                     const Uint16 messageID,
                                     const OFString &sopClassUID,
                                     DcmDataset *rspDataset,
                                     const Uint16 rspStatusCode,
                                     DcmDataset *statusDetail,
                                     const Uint16 numRemain,
                                     const Uint16 numComplete,
                                     const Uint16 numFail,
                                     const Uint16 numWarn)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_C_MoveRSP &moveRsp = response.msg.CMoveRSP;
  response.CommandField = DIMSE_C_MOVE_RSP;
  moveRsp.MessageIDBeingRespondedTo = messageID;
  moveRsp.DimseStatus = rspStatusCode;
  // Always send the optional field "Affected SOP Class UID"
  moveRsp.opts = O_MOVE_AFFECTEDSOPCLASSUID;
  OFStandard::strlcpy(moveRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(moveRsp.AffectedSOPClassUID));
  // Only send the other optional fields if needed
  if ( (numRemain != 0) || (numComplete != 0) || (numFail != 0) || (numWarn != 0) )
  {
    moveRsp.NumberOfRemainingSubOperations = numRemain;
    moveRsp.NumberOfCompletedSubOperations = numComplete;
    moveRsp.NumberOfFailedSubOperations = numFail;
    moveRsp.NumberOfWarningSubOperations = numWarn;
    moveRsp.opts |= O_MOVE_NUMBEROFREMAININGSUBOPERATIONS | O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS |
                    O_MOVE_NUMBEROFFAILEDSUBOPERATIONS | O_MOVE_NUMBEROFWARNINGSUBOPERATIONS;
  }

  if (rspDataset)
    moveRsp.DataSetType = DIMSE_DATASET_PRESENT;
  else
    moveRsp.DataSetType = DIMSE_DATASET_NULL;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-MOVE Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, rspDataset, presID));
  } else {
    DCMNET_INFO("Sending C-MOVE Response (" << DU_cmoveStatusString(rspStatusCode) << ")");
  }

  // Send response message with dataset
  cond = sendDIMSEMessage(presID, &response, rspDataset /* dataObject */, statusDetail);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-MOVE response: " << DimseCondition::dump(tempStr, cond));
  }

  return cond;
}