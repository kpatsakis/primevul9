OFCondition DcmSCP::receiveSTORERequestDataset(T_ASC_PresentationContextID *presID,
                                               T_DIMSE_C_StoreRQ &reqMessage,
                                               const OFString &filename)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (filename.empty())
    return EC_InvalidFilename;

  OFString tempStr;
  DcmOutputFileStream *filestream = NULL;
  // Receive dataset over the network and write it directly to a file
  OFCondition cond = DIMSE_createFilestream(filename.c_str(), &reqMessage, m_assoc, *presID,
                                            OFTrue /*writeMetaheader*/, &filestream);
  if (cond.good())
  {
    if (m_cfg->getProgressNotificationMode())
    {
      cond = DIMSE_receiveDataSetInFile(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, filestream, callbackRECEIVEProgress, this /*callbackData*/);
    } else {
      cond = DIMSE_receiveDataSetInFile(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, filestream, NULL /*callback*/, NULL /*callbackData*/);
    }
    delete filestream;
    if (cond.good())
    {
      DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID)
        << " and stored it directly to file");
    } else {
      DCMNET_ERROR("Unable to receive dataset on presentation context "
        << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
      // Delete created file in case of error
      OFStandard::deleteFile(filename);
    }

  } else {

    DCMNET_ERROR("Unable to receive dataset on presentation context "
      << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
    // Could not create the filestream, so ignore the dataset
    DIC_UL bytesRead = 0;
    DIC_UL pdvCount = 0;
    DCMNET_DEBUG("Ignoring incoming dataset and returning an error status to the SCU");
    cond = DIMSE_ignoreDataSet(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                               &bytesRead, &pdvCount);
    if (cond.good())
    {
      tempStr = "Cannot create file: " + filename;
      cond = makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, tempStr.c_str());
    }
  }
  return cond;
}