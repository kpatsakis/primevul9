static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
    /*
     * This function processes a DIMSE C-STORE-RQ command that was
     * received over the network connection.
     *
     * Parameters:
     *   assoc  - [in] The association (network connection to another DICOM application).
     *   msg    - [in] The DIMSE C-STORE-RQ message that was received.
     *   presID - [in] The ID of the presentation context which was specified in the PDV which contained
     *                 the DIMSE command.
     */
{
  OFCondition cond = EC_Normal;
  T_DIMSE_C_StoreRQ *req;
  char imageFileName[2048];

  // assign the actual information of the C-STORE-RQ command to a local variable
  req = &msg->msg.CStoreRQ;

  // if opt_ignore is set, the user requires that the data shall be received but not
  // stored. in this case, we want to create a corresponding temporary filename for
  // a file in which the data shall be stored temporarily. If this is not the case,
  // create a real filename (consisting of path and filename) for a real file.
  if (opt_ignore)
  {
#ifdef _WIN32
    tmpnam(imageFileName);
#else
    strcpy(imageFileName, NULL_DEVICE_NAME);
#endif
  }
  else
  {
    // 3 possibilities: create unique filenames (fn), create timestamp fn, create fn from SOP Instance UIDs
    if (opt_uniqueFilenames)
    {
      // create unique filename by generating a temporary UID and using ".X." as an infix
      char buf[70];
      dcmGenerateUniqueIdentifier(buf);
      sprintf(imageFileName, "%s%c%s.X.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"),
        buf, opt_fileNameExtension.c_str());
    }
    else if (opt_timeNames)
    {
      // create a name for the new file. pattern: "[YYYYMMDDHHMMSSMMM]_[NUMBER].MODALITY[EXTENSION]" (use current datetime)
      // get the current time (needed for file name)
      OFDateTime dateTime;
      dateTime.setCurrentDateTime();
      // used to hold prospective filename
      char cmpFileName[2048];
      // next if/else block generates prospective filename, that is compared to last written filename
      if (timeNameCounter == -1)
      {
        // timeNameCounter not set -> last written filename has to be without "serial number"
        sprintf(cmpFileName, "%04u%02u%02u%02u%02u%02u%03u.%s%s",
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      else
      {
        // counter was active before, so generate filename with "serial number" for comparison
        sprintf(cmpFileName, "%04u%02u%02u%02u%02u%02u%03u_%04u.%s%s", // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          timeNameCounter, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      if ( (outputFileNameArray.size()!=0) && (outputFileNameArray.back() == cmpFileName) )
      {
        // if this is not the first run and the prospective filename is equal to the last written filename
        // generate one with a serial number (incremented by 1)
        timeNameCounter++;
        sprintf(imageFileName, "%s%c%04u%02u%02u%02u%02u%02u%03u_%04u.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          timeNameCounter, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      else
      {
        // first run or filenames are different: create filename without serial number
        sprintf(imageFileName, "%s%c%04u%02u%02u%02u%02u%02u%03u.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(),dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
        // reset counter, because timestamp and therefore filename has changed
        timeNameCounter = -1;
      }
    }
    else
    {
      // don't create new UID, use the study instance UID as found in object
      sprintf(imageFileName, "%s%c%s.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"),
        req->AffectedSOPInstanceUID, opt_fileNameExtension.c_str());
    }
  }

  // dump some information if required
  OFString str;
  if (storescpLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    OFLOG_INFO(storescpLogger, "Received Store Request");
    OFLOG_DEBUG(storescpLogger, DIMSE_dumpMessage(str, *req, DIMSE_INCOMING, NULL, presID));
  } else {
    OFLOG_INFO(storescpLogger, "Received Store Request (MsgID " << req->MessageID << ", "
      << dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "OT") << ")");
  }

  // initialize some variables
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

  // define an address where the information which will be received over the network will be stored
  DcmDataset *dset = dcmff.getDataset();

  // if opt_bitPreserving is set, the user requires that the data shall be
  // written exactly as it was received. Depending on this option, function
  // DIMSE_storeProvider must be called with certain parameters.
  if (opt_bitPreserving)
  {
    cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader, NULL,
      storeSCPCallback, &callbackData, opt_blockMode, opt_dimse_timeout);
  }
  else
  {
    cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader, &dset,
      storeSCPCallback, &callbackData, opt_blockMode, opt_dimse_timeout);
  }

  // if some error occurred, dump corresponding information and remove the outfile if necessary
  if (cond.bad())
  {
    OFString temp_str;
    OFLOG_ERROR(storescpLogger, "Store SCP Failed: " << DimseCondition::dump(temp_str, cond));
    // remove file
    if (!opt_ignore)
    {
      if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0)
        OFStandard::deleteFile(imageFileName);
    }
  }
#ifdef _WIN32
  else if (opt_ignore)
  {
    if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0)
      OFStandard::deleteFile(imageFileName); // delete the temporary file
  }
#endif

  // if everything was successful so far and option --exec-on-reception is set,
  // we want to execute a certain command which was passed to the application
  if( cond.good() && opt_execOnReception != NULL )
    executeOnReception();

  // if everything was successful so far, go ahead and handle possible end-of-study events
  if( cond.good() )
    executeEndOfStudyEvents();

  // if opt_sleepAfter is set, the user requires that the application shall
  // sleep a certain amount of seconds after storing the instance data.
  if (opt_sleepAfter > 0)
  {
    OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepAfter));
  }

  // return return value
  return cond;
}