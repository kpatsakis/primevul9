static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_C_StoreRQ *request,
  T_ASC_PresentationContextID presId,
  const char *dbfolder,
  OFBool opt_bitpreserving,
  OFBool opt_correctUIDPadding)
{
    OFString str;
    OFLOG_INFO(dcmpsrcvLogger, "Received Store SCP:\n" << DIMSE_dumpMessage(str, *request, DIMSE_INCOMING));

    OFCondition cond = EC_Normal;
    char imageFileName[MAXPATHLEN+1];
    DcmFileFormat dcmff;

    // store SourceApplicationEntityTitle in metaheader
    if (assoc && assoc->params)
    {
      const char *aet = assoc->params->DULparams.callingAPTitle;
      if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
    }

    DcmDataset *dset = dcmff.getDataset();
    DIC_US status = STATUS_Success;
    DcmQueryRetrieveIndexDatabaseHandle *dbhandle = NULL;

    if (!dcmIsaStorageSOPClassUID(request->AffectedSOPClassUID))
    {
        /* callback will send back sop class not supported status */
        status = STATUS_STORE_Refused_SOPClassNotSupported;
        /* must still receive data */
        strcpy(imageFileName, NULL_DEVICE_NAME);
    }
    else
    {
      dbhandle = new DcmQueryRetrieveIndexDatabaseHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, cond);
      if (cond.bad())
      {
        OFLOG_ERROR(dcmpsrcvLogger, "Unable to access database '" << dbfolder << "'");
        /* must still receive data */
        strcpy(imageFileName, NULL_DEVICE_NAME);
        /* callback will send back out of resources status */
        status = STATUS_STORE_Refused_OutOfResources;
        dbhandle = NULL;
      }
      else
      {
        if (dbhandle->makeNewStoreFileName(
            request->AffectedSOPClassUID,
            request->AffectedSOPInstanceUID,
            imageFileName).bad())
        {
            OFLOG_ERROR(dcmpsrcvLogger, "storeSCP: Database: DB_makeNewStoreFileName Failed");
            /* must still receive data */
            strcpy(imageFileName, NULL_DEVICE_NAME);
            /* callback will send back out of resources status */
            status = STATUS_STORE_Refused_OutOfResources;
        }
      }
    }

#ifdef LOCK_IMAGE_FILES
    /* exclusively lock image file, but only on Unix systems -
         * on Win32 we would prevent ourselves from writing the file!
         */
#ifdef O_BINARY
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY), 0666);
#else
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC), 0666);
#endif

    dcmtk_flock(lockfd, LOCK_EX);
#endif

    /* we must still retrieve the data set even if some error has occured */
    StoreContext context(dbhandle, status, imageFileName, &dcmff, opt_correctUIDPadding);

    if (opt_bitpreserving)
    {
        cond = DIMSE_storeProvider(assoc, presId, request, imageFileName, 1,
                                   NULL, storeProgressCallback,
                                   (void*)&context, DIMSE_BLOCKING, 0);
    } else {
        cond = DIMSE_storeProvider(assoc, presId, request, (char *)NULL, 1,
                                   &dset, storeProgressCallback,
                                   (void*)&context, DIMSE_BLOCKING, 0);
    }
    errorCond(cond, "Store SCP Failed:");

    if (cond.bad() || (context.status != STATUS_Success))
    {
        /* remove file */
        if (strcpy(imageFileName, NULL_DEVICE_NAME) != 0)
        {
          OFLOG_INFO(dcmpsrcvLogger, "Store SCP: Deleting Image File: " << imageFileName);
          unlink(imageFileName);
        }
        if (dbhandle) dbhandle->pruneInvalidRecords();
    }

#ifdef LOCK_IMAGE_FILES
    /* unlock image file */
    dcmtk_flock(lockfd, LOCK_UN);
    close(lockfd);
#endif

    /* free DB handle */
    delete dbhandle;

    if (messageClient)
    {
      OFOStringStream out;
      Uint32 operationStatus = DVPSIPCMessage::statusError;
      if (cond.good())
      {
        if (context.status == STATUS_Success) operationStatus = DVPSIPCMessage::statusOK;
        else operationStatus = DVPSIPCMessage::statusWarning;
      }
      const char *sopClassName = dcmFindNameOfUID(request->AffectedSOPClassUID);
      const char *successName = "failed";
      if (operationStatus == DVPSIPCMessage::statusOK) successName = "successful";
      if (sopClassName==NULL) sopClassName = request->AffectedSOPClassUID;

      out << "DICOM C-STORE receipt " << successName << ": " << OFendl
          << "\tSOP class UID          : " << sopClassName << OFendl
          << "\tSOP instance UID       : " << request->AffectedSOPInstanceUID << OFendl;

      if (operationStatus == DVPSIPCMessage::statusOK)
      {
        unsigned long fileSize = 0;
        struct stat fileStat;
        if (0 == stat(imageFileName, &fileStat)) fileSize = fileStat.st_size;
        out
          << "\tTarget file path       : " << imageFileName << OFendl
          << "\tTarget file size (kB)  : " << (fileSize+1023)/1024 << OFendl;
      }

      out << "\tDIMSE presentation ctx : " << (int)presId << OFendl
          << "\tDIMSE message ID       : " << request->MessageID << OFendl
          << "\tDIMSE status           : " << DU_cstoreStatusString(context.status) << OFendl
          << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(out, theString)
      messageClient->notifyReceivedDICOMObject(operationStatus, theString);
      OFSTRINGSTREAM_FREESTR(theString)
    }

    return cond;
}