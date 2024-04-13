OFCondition DcmSCP::sendEVENTREPORTRequest(const T_ASC_PresentationContextID presID,
                                           const OFString &sopInstanceUID,
                                           const Uint16 messageID,
                                           const Uint16 eventTypeID,
                                           DcmDataset *reqDataset,
                                           Uint16 &rspStatusCode)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (sopInstanceUID.empty() || (reqDataset == NULL))
    return DIMSE_NULLKEY;

  // Prepare DIMSE data structures for issuing request
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message request;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&request, sizeof(request));
  T_DIMSE_N_EventReportRQ &eventReportReq = request.msg.NEventReportRQ;
  DcmDataset *statusDetail = NULL;

  request.CommandField = DIMSE_N_EVENT_REPORT_RQ;

  // Generate a new message ID (?)
  eventReportReq.MessageID = messageID;

  eventReportReq.DataSetType = DIMSE_DATASET_PRESENT;
  eventReportReq.EventTypeID = eventTypeID;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;

  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(eventReportReq.AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(eventReportReq.AffectedSOPClassUID));
  OFStandard::strlcpy(eventReportReq.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(eventReportReq.AffectedSOPInstanceUID));

  // Send request
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Request");
    // Output dataset only if trace level is enabled
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
    else
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Request (MsgID " << eventReportReq.MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &request, reqDataset);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  // Receive response
  T_DIMSE_Message response;
  cond = receiveDIMSECommand(&pcid, &response, &statusDetail, NULL /* commandSet */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Check command set
  if (response.CommandField == DIMSE_N_EVENT_REPORT_RSP)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received N-EVENT-REPORT Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received N-EVENT-REPORT Response (" << DU_neventReportStatusString(response.msg.NEventReportRSP.DimseStatus) << ")");
    }
  } else {
    DCMNET_ERROR("Expected N-EVENT-REPORT response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, response.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  // Set return value
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  rspStatusCode = eventReportRsp.DimseStatus;

  // Check whether there is a dataset to be received
  if (eventReportRsp.DataSetType == DIMSE_DATASET_PRESENT)
  {
    // this should never happen
    DcmDataset *tempDataset = NULL;
    T_ASC_PresentationContextID tempID;
    // Receive dataset
    cond = receiveDIMSEDataset(&tempID, &tempDataset);
    if (cond.good())
    {
      DCMNET_WARN("Received unexpected dataset after N-EVENT-REPORT response, ignoring");
      delete tempDataset;
    } else {
      DCMNET_ERROR("Failed receiving unexpected dataset after N-EVENT-REPORT response: "
        << DimseCondition::dump(tempStr, cond));
      return DIMSE_BADDATA;
    }
  }

  // Check whether the message ID being responded to is equal to the message ID of the request
  if (eventReportRsp.MessageIDBeingRespondedTo != eventReportReq.MessageID)
  {
    DCMNET_ERROR("Received response with wrong message ID (" << eventReportRsp.MessageIDBeingRespondedTo
      << " instead of " << eventReportReq.MessageID << ")");
    return DIMSE_BADMESSAGE;
  }

  return cond;
}