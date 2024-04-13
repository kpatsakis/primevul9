storeSCPCallback(
    /* in */
    void *callbackData,
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if ((opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
        (opt_abortAfterStore && progress->state == DIMSE_StoreEnd)) {
        OFLOG_INFO(movescuLogger, "ABORT initiated (due to command line options)");
        ASC_abortAssociation(OFstatic_cast(StoreCallbackData*, callbackData)->assoc);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
    }

    if (opt_sleepDuring > 0)
    {
      OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
    }

    // dump some information if required (depending on the progress state)
    // We can't use oflog for the pdu output, but we use a special logger for
    // generating this output. If it is set to level "INFO" we generate the
    // output, if it's set to "DEBUG" then we'll assume that there is debug output
    // generated for each PDU elsewhere.
    OFLogger progressLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION ".progress");
    if (progressLogger.getChainedLogLevel() == OFLogger::INFO_LOG_LEVEL)
    {
      switch (progress->state)
      {
        case DIMSE_StoreBegin:
          COUT << "RECV: ";
          break;
        case DIMSE_StoreEnd:
          COUT << OFendl;
          break;
        default:
          COUT << '.';
          break;
      }
      COUT.flush();
    }

    if (progress->state == DIMSE_StoreEnd)
    {
       *statusDetail = NULL;    /* no status detail */

       /* could save the image somewhere else, put it in database, etc */
       /*
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
       // rsp->DimseStatus = STATUS_Success;

       if ((imageDataSet != NULL) && (*imageDataSet != NULL) && !opt_bitPreserving && !opt_ignore)
       {
         StoreCallbackData *cbdata = OFstatic_cast(StoreCallbackData*, callbackData);
         /* create full path name for the output file */
         OFString ofname;
         OFStandard::combineDirAndFilename(ofname, opt_outputDirectory, cbdata->imageFileName, OFTrue /* allowEmptyDirName */);

         E_TransferSyntax xfer = opt_writeTransferSyntax;
         if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

         OFCondition cond = cbdata->dcmff->saveFile(ofname.c_str(), xfer, opt_sequenceType, opt_groupLength,
           opt_paddingType, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad),
           (opt_useMetaheader) ? EWM_fileformat : EWM_dataset);
         if (cond.bad())
         {
           OFLOG_ERROR(movescuLogger, "cannot write DICOM file: " << ofname);
           rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
         }

        /* should really check the image to make sure it is consistent,
         * that its sopClass and sopInstance correspond with those in
         * the request.
         */
        if ((rsp->DimseStatus == STATUS_Success) && !opt_ignore)
        {
          /* which SOP class and SOP instance ? */
          if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
          {
             OFLOG_FATAL(movescuLogger, "bad DICOM file: " << imageFileName);
             rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          }
          else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
          {
            rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
          else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
          {
            rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
        }
      }
    }
}