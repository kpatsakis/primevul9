processCommands(T_ASC_Association * assoc)
    /*
     * This function receives DIMSE commands over the network connection
     * and handles these commands correspondingly. Note that in case of
     * storescp only C-ECHO-RQ and C-STORE-RQ commands can be processed.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     */
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID = 0;
  DcmDataset *statusDetail = NULL;

  // start a loop to be able to receive more than one DIMSE command
  while( cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES )
  {
    // receive a DIMSE command over the network
    if( opt_endOfStudyTimeout == -1 )
      cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, &statusDetail);
    else
      cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, OFstatic_cast(int, opt_endOfStudyTimeout), &presID, &msg, &statusDetail);

    // check what kind of error occurred. If no data was
    // received, check if certain other conditions are met
    if( cond == DIMSE_NODATAAVAILABLE )
    {
      // If in addition to the fact that no data was received also option --eostudy-timeout is set and
      // if at the same time there is still a study which is considered to be open (i.e. we were actually
      // expecting to receive more objects that belong to this study) (this is the case if lastStudyInstanceUID
      // does not equal NULL), we have to consider that all objects for the current study have been received.
      // In such an "end-of-study" case, we might have to execute certain optional functions which were specified
      // by the user through command line options passed to storescp.
      if( opt_endOfStudyTimeout != -1 && !lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occurred through a timeout event.
        // This knowledge will be necessary in function renameOnEndOFStudy().
        endOfStudyThroughTimeoutEvent = OFTrue;

        // before we actually execute those optional functions, we need to determine the path and name
        // of the subdirectory into which the DICOM files for the last study were written.
        lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

        // now we can finally handle end-of-study events which might have to be executed
        executeEndOfStudyEvents();

        // also, we need to clear lastStudyInstanceUID to indicate
        // that the last study is not considered to be open any more.
        lastStudyInstanceUID.clear();

        // also, we need to clear subdirectoryPathAndName
        subdirectoryPathAndName.clear();

        // reset the endOfStudyThroughTimeoutEvent variable.
        endOfStudyThroughTimeoutEvent = OFFalse;
      }
    }

    // if the command which was received has extra status
    // detail information, dump this information
    if (statusDetail != NULL)
    {
      OFLOG_DEBUG(storescpLogger, "Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
      delete statusDetail;
    }

    // check if peer did release or abort, or if we have a valid message
    if (cond == EC_Normal)
    {
      // in case we received a valid message, process this command
      // note that storescp can only process a C-ECHO-RQ and a C-STORE-RQ
      switch (msg.CommandField)
      {
        case DIMSE_C_ECHO_RQ:
          // process C-ECHO-Request
          cond = echoSCP(assoc, &msg, presID);
          break;
        case DIMSE_C_STORE_RQ:
          // process C-STORE-Request
          cond = storeSCP(assoc, &msg, presID);
          break;
        default:
          OFString tempStr;
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          OFLOG_ERROR(storescpLogger, "Expected C-ECHO or C-STORE request but received DIMSE command 0x"
               << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
               << OFstatic_cast(unsigned, msg.CommandField));
          OFLOG_DEBUG(storescpLogger, DIMSE_dumpMessage(tempStr, msg, DIMSE_INCOMING, NULL, presID));
          break;
      }
    }
  }
  return cond;
}