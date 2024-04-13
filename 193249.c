static OFCondition acceptAssociation(T_ASC_Network *net, DcmAssociationConfiguration& asccfg)
{
  char buf[BUFSIZ];
  T_ASC_Association *assoc;
  OFCondition cond;
  OFString sprofile;
  OFString temp_str;

#ifdef PRIVATE_STORESCP_VARIABLES
  PRIVATE_STORESCP_VARIABLES
#endif

  const char* knownAbstractSyntaxes[] =
  {
    UID_VerificationSOPClass
  };

  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  // try to receive an association. Here we either want to use blocking or
  // non-blocking, depending on if the option --eostudy-timeout is set.
  if( opt_endOfStudyTimeout == -1 )
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection);
  else
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection, DUL_NOBLOCK, OFstatic_cast(int, opt_endOfStudyTimeout));

  if (cond.code() == DULC_FORKEDCHILD)
  {
    // OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
    goto cleanup;
  }

  // if some kind of error occurred, take care of it
  if (cond.bad())
  {
    // check what kind of error occurred. If no association was
    // received, check if certain other conditions are met
    if( cond == DUL_NOASSOCIATIONREQUEST )
    {
      // If in addition to the fact that no association was received also option --eostudy-timeout is set
      // and if at the same time there is still a study which is considered to be open (i.e. we were actually
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
    // If something else was wrong we might have to dump an error message.
    else
    {
      OFLOG_ERROR(storescpLogger, "Receiving Association failed: " << DimseCondition::dump(temp_str, cond));
    }

    // no matter what kind of error occurred, we need to do a cleanup
    goto cleanup;
  }

#if defined(HAVE_FORK) || defined(_WIN32)
  if (opt_forkMode)
    OFLOG_INFO(storescpLogger, "Association Received in " << (DUL_processIsForkedChild() ? "child" : "parent")
        << " process (pid: " << OFStandard::getProcessID() << ")");
  else
#endif
  OFLOG_INFO(storescpLogger, "Association Received");

  /* dump presentation contexts if required */
  if (opt_showPresentationContexts)
    OFLOG_INFO(storescpLogger, "Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));
  else
    OFLOG_DEBUG(storescpLogger, "Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));

  if (opt_refuseAssociation)
  {
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    OFLOG_INFO(storescpLogger, "Refusing Association (forced via command line)");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      OFLOG_ERROR(storescpLogger, "Association Reject Failed: " << DimseCondition::dump(temp_str, cond));
    }
    goto cleanup;
  }

  switch (opt_networkTransferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_JPEGProcess14SV1:
      /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
      transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess1:
      /* we prefer JPEGBaseline (default lossy for 8 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess2_4:
      /* we prefer JPEGExtended (default lossy for 12 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEG2000LosslessOnly:
      /* we prefer JPEG2000 Lossless */
      transferSyntaxes[0] = UID_JPEG2000LosslessOnlyTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEG2000:
      /* we prefer JPEG2000 Lossy */
      transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGLSLossless:
      /* we prefer JPEG-LS Lossless */
      transferSyntaxes[0] = UID_JPEGLSLosslessTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGLSLossy:
      /* we prefer JPEG-LS Lossy */
      transferSyntaxes[0] = UID_JPEGLSLossyTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG2MainProfileAtMainLevel:
      /* we prefer MPEG2 MP@ML */
      transferSyntaxes[0] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG2MainProfileAtHighLevel:
      /* we prefer MPEG2 MP@HL */
      transferSyntaxes[0] = UID_MPEG2MainProfileAtHighLevelTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG4HighProfileLevel4_1:
      /* we prefer MPEG4 HP/L4.1 */
      transferSyntaxes[0] = UID_MPEG4HighProfileLevel4_1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG4BDcompatibleHighProfileLevel4_1:
      /* we prefer MPEG4 BD HP/L4.1 */
      transferSyntaxes[0] = UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_RLELossless:
      /* we prefer RLE Lossless */
      transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
      /* we prefer Deflated Explicit VR Little Endian */
      transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#endif
    default:
      if (opt_acceptAllXfers)
      {
        /* we accept all supported transfer syntaxes
         * (similar to "AnyTransferSyntax" in "storescp.cfg")
         */
        transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
        transferSyntaxes[1] = UID_JPEG2000LosslessOnlyTransferSyntax;
        transferSyntaxes[2] = UID_JPEGProcess2_4TransferSyntax;
        transferSyntaxes[3] = UID_JPEGProcess1TransferSyntax;
        transferSyntaxes[4] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[5] = UID_JPEGLSLossyTransferSyntax;
        transferSyntaxes[6] = UID_JPEGLSLosslessTransferSyntax;
        transferSyntaxes[7] = UID_RLELosslessTransferSyntax;
        transferSyntaxes[8] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
        transferSyntaxes[9] = UID_MPEG2MainProfileAtHighLevelTransferSyntax;
        transferSyntaxes[10] = UID_MPEG4HighProfileLevel4_1TransferSyntax;
        transferSyntaxes[11] = UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax;
        transferSyntaxes[12] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
        if (gLocalByteOrder == EBO_LittleEndian)
        {
          transferSyntaxes[13] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[14] = UID_BigEndianExplicitTransferSyntax;
        } else {
          transferSyntaxes[13] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[14] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[15] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 16;
      } else {
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        }
        else
        {
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
      }
      break;
  }

  if (opt_profileName)
  {
    /* perform name mangling for config file key */
    const unsigned char *c = OFreinterpret_cast(const unsigned char *, opt_profileName);
    while (*c)
    {
      if (!isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
      ++c;
    }

    /* set presentation contexts as defined in config file */
    cond = asccfg.evaluateAssociationParameters(sprofile.c_str(), *assoc);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }
  }
  else
  {
    /* accept the Verification SOP Class if presented */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }

    /* the array of Storage SOP Class UIDs comes from dcuid.h */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs, transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }

    if (opt_promiscuous)
    {
      /* accept everything not known not to be a storage SOP class */
      cond = acceptUnknownContextsWithPreferredTransferSyntaxes(
        assoc->params, transferSyntaxes, numTransferSyntaxes);
      if (cond.bad())
      {
        OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
        goto cleanup;
      }
    }
  }

  /* set our app title */
  ASC_setAPTitles(assoc->params, NULL, NULL, opt_respondingAETitle);

  /* acknowledge or reject this association */
  cond = ASC_getApplicationContextName(assoc->params, buf);
  if ((cond.bad()) || strcmp(buf, UID_StandardApplicationContext) != 0)
  {
    /* reject: the application context name is not supported */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
    };

    OFLOG_INFO(storescpLogger, "Association Rejected: Bad Application Context Name: " << buf);
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
    }
    goto cleanup;

  }
  else if (opt_rejectWithoutImplementationUID && strlen(assoc->params->theirImplementationClassUID) == 0)
  {
    /* reject: the no implementation Class UID provided */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    OFLOG_INFO(storescpLogger, "Association Rejected: No Implementation Class UID provided");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
    }
    goto cleanup;
  }
  else
  {
#ifdef PRIVATE_STORESCP_CODE
    PRIVATE_STORESCP_CODE
#endif
    cond = ASC_acknowledgeAssociation(assoc);
    if (cond.bad())
    {
      OFLOG_ERROR(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }
    OFLOG_INFO(storescpLogger, "Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")");
    if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
      OFLOG_INFO(storescpLogger, "    (but no valid presentation contexts)");
    /* dump the presentation contexts which have been accepted/refused */
    if (opt_showPresentationContexts)
      OFLOG_INFO(storescpLogger, ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));
    else
      OFLOG_DEBUG(storescpLogger, ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));
  }

#ifdef BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX
  /* active the dcmPeerRequiresExactUIDCopy workaround code
   * (see comments in dimse.h) for a implementation class UID
   * prefix known to exhibit the buggy behaviour.
   */
  if (0 == strncmp(assoc->params->theirImplementationClassUID,
      BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX,
      strlen(BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX)))
  {
    dcmEnableAutomaticInputDataCorrection.set(OFFalse);
    dcmPeerRequiresExactUIDCopy.set(OFTrue);
  }
#endif

  // store previous values for later use
  lastCallingAETitle = callingAETitle;
  lastCalledAETitle = calledAETitle;
  lastCallingPresentationAddress = callingPresentationAddress;
  // store calling and called aetitle in global variables to enable
  // the --exec options using them. Enclose in quotation marks because
  // aetitles may contain space characters.
  DIC_AE callingTitle;
  DIC_AE calledTitle;
  if (ASC_getAPTitles(assoc->params, callingTitle, calledTitle, NULL).good())
  {
    callingAETitle = "\"";
    callingAETitle += OFSTRING_GUARD(callingTitle);
    callingAETitle += "\"";
    calledAETitle = "\"";
    calledAETitle += OFSTRING_GUARD(calledTitle);
    calledAETitle += "\"";
  }
  else
  {
    // should never happen
    callingAETitle.clear();
    calledAETitle.clear();
  }
  // store calling presentation address (i.e. remote hostname)
  callingPresentationAddress = OFSTRING_GUARD(assoc->params->DULparams.callingPresentationAddress);

  /* now do the real work, i.e. receive DIMSE commands over the network connection */
  /* which was established and handle these commands correspondingly. In case of */
  /* storescp only C-ECHO-RQ and C-STORE-RQ commands can be processed. */
  cond = processCommands(assoc);

  if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    OFLOG_INFO(storescpLogger, "Association Release");
    cond = ASC_acknowledgeRelease(assoc);
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
    OFLOG_INFO(storescpLogger, "Association Aborted");
  }
  else
  {
    OFLOG_ERROR(storescpLogger, "DIMSE failure (aborting association): " << DimseCondition::dump(temp_str, cond));
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

cleanup:

  if (cond.code() == DULC_FORKEDCHILD) return cond;

  cond = ASC_dropSCPAssociation(assoc);
  if (cond.bad())
  {
    OFLOG_FATAL(storescpLogger, DimseCondition::dump(temp_str, cond));
    exit(1);
  }
  cond = ASC_destroyAssociation(&assoc);
  if (cond.bad())
  {
    OFLOG_FATAL(storescpLogger, DimseCondition::dump(temp_str, cond));
    exit(1);
  }

  return cond;
}