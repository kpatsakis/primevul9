OFCondition DcmSCP::receiveSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
                                        const T_ASC_PresentationContextID presID,
                                        DcmDataset *&reqDataset)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  // Remember the passed dataset pointer
  DcmDataset *dataset = reqDataset;

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

  // Receive dataset (in memory)
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive C-STORE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return cond;
  }

  // Output request message only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    if (dataset != reqDataset)
    {
      // Free memory allocated by receiveDIMSEDataset()
      delete dataset;
    }
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Set return value
  reqDataset = dataset;

  return cond;
}