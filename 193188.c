OFCondition DcmSCP::receiveSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
                                        const T_ASC_PresentationContextID presID,
                                        const OFString &filename)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  // Use presentation context ID of the command set as a default
  T_ASC_PresentationContextID presIDdset = presID;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    DCMNET_INFO("Received C-STORE Request");
  else
    DCMNET_INFO("Received C-STORE Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received C-STORE request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset (directly to file)
  cond = receiveSTORERequestDataset(&presIDdset, reqMessage, filename);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive C-STORE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return cond;
  }

  // Output request message only if trace level is enabled
  DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  return cond;
}