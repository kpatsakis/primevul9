storeSCPCallback(
    void *callbackData,
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ *req,
    char * /*imageFileName*/, DcmDataset **imageDataSet,
    T_DIMSE_C_StoreRSP *rsp,
    DcmDataset **statusDetail)
    /*
     * This function.is used to indicate progress when storescp receives instance data over the
     * network. On the final call to this function (identified by progress->state == DIMSE_StoreEnd)
     * this function will store the data set which was received over the network to a file.
     * Earlier calls to this function will simply cause some information to be dumped to stdout.
     *
     * Parameters:
     *   callbackData  - [in] data for this callback function
     *   progress      - [in] The state of progress. (identifies if this is the initial or final call
     *                   to this function, or a call in between these two calls.
     *   req           - [in] The original store request message.
     *   imageFileName - [in] The path to and name of the file the information shall be written to.
     *   imageDataSet  - [in] The data set which shall be stored in the image file
     *   rsp           - [inout] the C-STORE-RSP message (will be sent after the call to this function)
     *   statusDetail  - [inout] This variable can be used to capture detailed information with regard to
     *                   the status information which is captured in the status element (0000,0900). Note
     *                   that this function does specify any such information, the pointer will be set to NULL.
     */
{
  DIC_UI sopClass;
  DIC_UI sopInstance;

  // determine if the association shall be aborted
  if( (opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
      (opt_abortAfterStore && progress->state == DIMSE_StoreEnd) )
  {
    OFLOG_INFO(storescpLogger, "ABORT initiated (due to command line options)");
    ASC_abortAssociation((OFstatic_cast(StoreCallbackData*, callbackData))->assoc);
    rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
    return;
  }

  // if opt_sleepAfter is set, the user requires that the application shall
  // sleep a certain amount of seconds after having received one PDU.
  if (opt_sleepDuring > 0)
  {
    OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
  }

  // dump some information if required (depending on the progress state)
  // We can't use oflog for the PDU output, but we use a special logger for
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

  // if this is the final call of this function, save the data which was received to a file
  // (note that we could also save the image somewhere else, put it in database, etc.)
  if (progress->state == DIMSE_StoreEnd)
  {
    OFString tmpStr;

    // do not send status detail information
    *statusDetail = NULL;

    // remember callback data
    StoreCallbackData *cbdata = OFstatic_cast(StoreCallbackData *, callbackData);

    // Concerning the following line: an appropriate status code is already set in the resp structure,
    // it need not be success. For example, if the caller has already detected an out of resources problem
    // then the status will reflect this.  The callback function is still called to allow cleanup.
    //rsp->DimseStatus = STATUS_Success;

    // we want to write the received information to a file only if this information
    // is present and the options opt_bitPreserving and opt_ignore are not set.
    if ((imageDataSet != NULL) && (*imageDataSet != NULL) && !opt_bitPreserving && !opt_ignore)
    {
      OFString fileName;

      // in case one of the --sort-xxx options is set, we need to perform some particular steps to
      // determine the actual name of the output file
      if (opt_sortStudyMode != ESM_None)
      {
        // determine the study instance UID in the (current) DICOM object that has just been received
        OFString currentStudyInstanceUID;
        if ((*imageDataSet)->findAndGetOFString(DCM_StudyInstanceUID, currentStudyInstanceUID).bad() || currentStudyInstanceUID.empty())
        {
          OFLOG_ERROR(storescpLogger, "element StudyInstanceUID " << DCM_StudyInstanceUID << " absent or empty in data set");
          rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          return;
        }

        // if --sort-on-patientname is active, we need to extract the
        // patient's name (format: last_name^first_name)
        OFString currentPatientName;
        if (opt_sortStudyMode == ESM_PatientName)
        {
          OFString tmpName;
          if ((*imageDataSet)->findAndGetOFString(DCM_PatientName, tmpName).bad() || tmpName.empty())
          {
            // default if patient name is missing or empty
            tmpName = "ANONYMOUS";
            OFLOG_WARN(storescpLogger, "element PatientName " << DCM_PatientName << " absent or empty in data set, using '"
                 << tmpName << "' instead");
          }

          /* substitute non-ASCII characters in patient name to ASCII "equivalent" */
          const size_t length = tmpName.length();
          for (size_t i = 0; i < length; i++)
            mapCharacterAndAppendToString(tmpName[i], currentPatientName);
        }

        // if this is the first DICOM object that was received or if the study instance UID in the
        // current DICOM object does not equal the last object's study instance UID we need to create
        // a new subdirectory in which the current DICOM object will be stored
        if (lastStudyInstanceUID.empty() || (lastStudyInstanceUID != currentStudyInstanceUID))
        {
          // if lastStudyInstanceUID is non-empty, we have just completed receiving all objects for one
          // study. In such a case, we need to set a certain indicator variable (lastStudySubdirectoryPathAndName),
          // so that we know that executeOnEndOfStudy() might have to be executed later. In detail, this indicator
          // variable will contain the path and name of the last study's subdirectory, so that we can still remember
          // this directory, when we execute executeOnEndOfStudy(). The memory that is allocated for this variable
          // here will be freed after the execution of executeOnEndOfStudy().
          if (!lastStudyInstanceUID.empty())
            lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

          // create the new lastStudyInstanceUID value according to the value in the current DICOM object
          lastStudyInstanceUID = currentStudyInstanceUID;

          // get the current time (needed for subdirectory name)
          OFDateTime dateTime;
          dateTime.setCurrentDateTime();

          // create a name for the new subdirectory.
          char timestamp[32];
          sprintf(timestamp, "%04u%02u%02u_%02u%02u%02u%03u",
            dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
            dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond());

          OFString subdirectoryName;
          switch (opt_sortStudyMode)
          {
            case ESM_Timestamp:
              // pattern: "[prefix]_[YYYYMMDD]_[HHMMSSMMM]"
              subdirectoryName = opt_sortStudyDirPrefix;
              if (!subdirectoryName.empty())
                subdirectoryName += '_';
              subdirectoryName += timestamp;
              break;
            case ESM_StudyInstanceUID:
              // pattern: "[prefix]_[Study Instance UID]"
              subdirectoryName = opt_sortStudyDirPrefix;
              if (!subdirectoryName.empty())
                subdirectoryName += '_';
              subdirectoryName += currentStudyInstanceUID;
              break;
            case ESM_PatientName:
              // pattern: "[Patient's Name]_[YYYYMMDD]_[HHMMSSMMM]"
              subdirectoryName = currentPatientName;
              subdirectoryName += '_';
              subdirectoryName += timestamp;
              break;
            case ESM_None:
              break;
          }

          // create subdirectoryPathAndName (string with full path to new subdirectory)
          OFStandard::combineDirAndFilename(subdirectoryPathAndName, OFStandard::getDirNameFromPath(tmpStr, cbdata->imageFileName), subdirectoryName);

          // check if the subdirectory already exists
          // if it already exists dump a warning
          if( OFStandard::dirExists(subdirectoryPathAndName) )
            OFLOG_WARN(storescpLogger, "subdirectory for study already exists: " << subdirectoryPathAndName);
          else
          {
            // if it does not exist create it
            OFLOG_INFO(storescpLogger, "creating new subdirectory for study: " << subdirectoryPathAndName);
#ifdef HAVE_WINDOWS_H
            if( _mkdir( subdirectoryPathAndName.c_str() ) == -1 )
#else
            if( mkdir( subdirectoryPathAndName.c_str(), S_IRWXU | S_IRWXG | S_IRWXO ) == -1 )
#endif
            {
              OFLOG_ERROR(storescpLogger, "could not create subdirectory for study: " << subdirectoryPathAndName);
              rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
              return;
            }
            // all objects of a study have been received, so a new subdirectory is started.
            // ->timename counter can be reset, because the next filename can't cause a duplicate.
            // if no reset would be done, files of a new study (->new directory) would start with a counter in filename
            if (opt_timeNames)
              timeNameCounter = -1;
          }
        }

        // integrate subdirectory name into file name (note that cbdata->imageFileName currently contains both
        // path and file name; however, the path refers to the output directory captured in opt_outputDirectory)
        OFStandard::combineDirAndFilename(fileName, subdirectoryPathAndName, OFStandard::getFilenameFromPath(tmpStr, cbdata->imageFileName));

        // update global variable outputFileNameArray
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        outputFileNameArray.push_back(tmpStr);
      }
      // if no --sort-xxx option is set, the determination of the output file name is simple
      else
      {
        fileName = cbdata->imageFileName;

        // update global variables outputFileNameArray
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        outputFileNameArray.push_back(OFStandard::getFilenameFromPath(tmpStr, fileName));
      }

      // determine the transfer syntax which shall be used to write the information to the file
      E_TransferSyntax xfer = opt_writeTransferSyntax;
      if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

      // store file either with meta header or as pure dataset
      OFLOG_INFO(storescpLogger, "storing DICOM file: " << fileName);
      if (OFStandard::fileExists(fileName))
      {
        OFLOG_WARN(storescpLogger, "DICOM file already exists, overwriting: " << fileName);
      }
      OFCondition cond = cbdata->dcmff->saveFile(fileName.c_str(), xfer, opt_sequenceType, opt_groupLength,
          opt_paddingType, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad),
          (opt_useMetaheader) ? EWM_fileformat : EWM_dataset);
      if (cond.bad())
      {
        OFLOG_ERROR(storescpLogger, "cannot write DICOM file: " << fileName << ": " << cond.text());
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
      }

      // check the image to make sure it is consistent, i.e. that its sopClass and sopInstance correspond
      // to those mentioned in the request. If not, set the status in the response message variable.
      if ((rsp->DimseStatus == STATUS_Success)&&(!opt_ignore))
      {
        // which SOP class and SOP instance ?
        if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
        {
           OFLOG_ERROR(storescpLogger, "bad DICOM file: " << fileName);
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

    // in case opt_bitPreserving is set, do some other things
    if( opt_bitPreserving )
    {
      // we need to set outputFileNameArray and outputFileNameArrayCnt to be
      // able to perform the placeholder substitution in executeOnReception()
      outputFileNameArray.push_back(OFStandard::getFilenameFromPath(tmpStr, cbdata->imageFileName));
    }
  }
}