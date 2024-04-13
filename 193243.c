OFCondition DcmSCP::receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                        DcmDataset **dataObject)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  /* call the corresponding DIMSE function to receive the dataset */
  if (m_cfg->getProgressNotificationMode())
  {
    cond = DIMSE_receiveDataSetInMemory(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, dataObject, callbackRECEIVEProgress, this /*callbackData*/);
  } else {
    cond = DIMSE_receiveDataSetInMemory(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, dataObject, NULL /*callback*/, NULL /*callbackData*/);
  }

  if (cond.good())
  {
    DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID));
  } else {
    OFString tempStr;
    DCMNET_ERROR("Unable to receive dataset on presentation context "
      << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
  }
  return cond;
}