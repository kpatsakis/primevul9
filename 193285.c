OFCondition DcmSCP::sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                                     T_DIMSE_Message *message,
                                     DcmDataset *dataObject,
                                     DcmDataset *statusDetail,
                                     DcmDataset **commandSet)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (message == NULL)
    return DIMSE_NULLKEY;

  OFCondition cond;
  /* call the corresponding DIMSE function to sent the message */
  if (m_cfg->getProgressNotificationMode())
  {
    cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, message, statusDetail, dataObject,
                                            callbackSENDProgress, this /*callbackData*/, commandSet);
  } else {
    cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, message, statusDetail, dataObject,
                                            NULL /*callback*/, NULL /*callbackData*/, commandSet);
  }
  return cond;
}