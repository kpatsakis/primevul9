OFCondition DcmSCP::handleEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &reqMessage,
                                             const T_ASC_PresentationContextID presID,
                                             DcmDataset *&reqDataset,
                                             Uint16 &eventTypeID)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  DcmDataset *dataset = NULL;
  // DcmDataset *statusDetail = NULL; // TODO: do we need this and if so, how do we get it?
  Uint16 rspStatusCode = 0;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_INFO("Received N-EVENT-REPORT Request");
  else
    DCMNET_INFO("Received N-EVENT-REPORT Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received N-EVENT-REPORT request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive N-EVENT-REPORT dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return DIMSE_BADDATA;
  }

  // Output dataset only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Check the request message and dataset and return the DIMSE status code to be used
  rspStatusCode = checkEVENTREPORTRequest(reqMessage, dataset);

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  response.CommandField = DIMSE_N_EVENT_REPORT_RSP;
  eventReportRsp.MessageIDBeingRespondedTo = reqMessage.MessageID;
  eventReportRsp.DimseStatus = rspStatusCode;
  eventReportRsp.DataSetType = DIMSE_DATASET_NULL;
  // Do not send any optional fields
  eventReportRsp.opts = 0;
  eventReportRsp.AffectedSOPClassUID[0] = 0;
  eventReportRsp.AffectedSOPInstanceUID[0] = 0;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Response (" << DU_neventReportStatusString(rspStatusCode) << ")");
  }
  // Send response message
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT response: " << DimseCondition::dump(tempStr, cond));
    delete dataset;
    return cond;
  }

  // Set return values
  reqDataset = dataset;
  eventTypeID = reqMessage.EventTypeID;

  return cond;
}