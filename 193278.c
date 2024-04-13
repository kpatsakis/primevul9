static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];

    req = &msg->msg.CStoreRQ;

    if (opt_ignore)
    {
#ifdef _WIN32
        tmpnam(imageFileName);
#else
        strcpy(imageFileName, NULL_DEVICE_NAME);
#endif
    } else {
        sprintf(imageFileName, "%s.%s",
            dcmSOPClassUIDToModality(req->AffectedSOPClassUID),
            req->AffectedSOPInstanceUID);
    }

    OFString temp_str;
    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        OFLOG_INFO(movescuLogger, "Received Store Request");
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presID));
    } else {
        OFLOG_INFO(movescuLogger, "Received Store Request (MsgID " << req->MessageID << ", "
            << dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "OT") << ")");
    }

    StoreCallbackData callbackData;
    callbackData.assoc = assoc;
    callbackData.imageFileName = imageFileName;
    DcmFileFormat dcmff;
    callbackData.dcmff = &dcmff;

    // store SourceApplicationEntityTitle in metaheader
    if (assoc && assoc->params)
    {
      const char *aet = assoc->params->DULparams.callingAPTitle;
      if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
    }

    DcmDataset *dset = dcmff.getDataset();

    if (opt_bitPreserving)
    {
      cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,
        NULL, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), opt_blockMode, opt_dimse_timeout);
    } else {
      cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader,
        &dset, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), opt_blockMode, opt_dimse_timeout);
    }

    if (cond.bad())
    {
      OFLOG_ERROR(movescuLogger, "Store SCP Failed: " << DimseCondition::dump(temp_str, cond));
      /* remove file */
      if (!opt_ignore)
      {
        if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName);
      }
#ifdef _WIN32
    } else if (opt_ignore) {
        if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName); // delete the temporary file
#endif
    }

    if (opt_sleepAfter > 0)
    {
      OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
    }
    return cond;
}