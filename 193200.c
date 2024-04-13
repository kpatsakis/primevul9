static void handleClient(
  T_ASC_Association **assoc,
  const char *dbfolder,
  OFBool opt_bitpreserving,
  OFBool useTLS,
  OFBool opt_correctUIDPadding)
{
  OFCondition cond = ASC_acknowledgeAssociation(*assoc);
  if (! errorCond(cond, "Cannot acknowledge association:"))
  {
    OFLOG_INFO(dcmpsrcvLogger, "Association Acknowledged (Max Send PDV: " << (*assoc)->sendPDVLength << ")"
        << (ASC_countAcceptedPresentationContexts((*assoc)->params) == 0 ? "    (but no valid presentation contexts)" : ""));

    if (messageClient)
    {
      // notify about successfully negotiated association
      OFOStringStream out;
      OFString temp_str;
      out << "DIMSE Association Acknowledged:" << OFendl
          << "  calling presentation address: " << (*assoc)->params->DULparams.callingPresentationAddress << OFendl
          << "  calling AE title: " << (*assoc)->params->DULparams.callingAPTitle << OFendl
          << "  called AE title: " << (*assoc)->params->DULparams.calledAPTitle << OFendl
          << "  max send PDV: " << (*assoc)->sendPDVLength << OFendl
          << "  presentation contexts: " << ASC_countAcceptedPresentationContexts((*assoc)->params) << OFendl;
      out << ASC_dumpConnectionParameters(temp_str, *assoc) << OFendl;
      out << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(out, theString)
      if (useTLS)
        messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
        else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
      OFSTRINGSTREAM_FREESTR(theString)
    }

    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;
    cond = EC_Normal;

    /* do real work */
    while (cond.good())
    {
      cond = DIMSE_receiveCommand(*assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);
      /* did peer release, abort, or do we have a valid message ? */

      if (cond.good())
      {
          /* process command */
          switch (msg.CommandField)
          {
            case DIMSE_C_ECHO_RQ:
              cond = echoSCP(*assoc, &msg.msg.CEchoRQ, presID);
              break;
            case DIMSE_C_STORE_RQ:
              cond = storeSCP(*assoc, &msg.msg.CStoreRQ, presID, dbfolder, opt_bitpreserving, opt_correctUIDPadding);
              break;
            default:
              cond = DIMSE_BADCOMMANDTYPE; /* unsupported command */
              OFLOG_ERROR(dcmpsrcvLogger, "Cannot handle command: 0x" << STD_NAMESPACE hex << (unsigned)msg.CommandField);
              break;
          }
      }
      else
      {
         /* finish processing loop */
      }
    } /* while */

    /* close association */
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {
      OFLOG_INFO(dcmpsrcvLogger, "Association Release");
      cond = ASC_acknowledgeRelease(*assoc);
      errorCond(cond, "Cannot release association:");
      if (messageClient) messageClient->notifyConnectionClosed(DVPSIPCMessage::statusOK);
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
      OFLOG_INFO(dcmpsrcvLogger, "Association Aborted");
      if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusWarning, "DIMSE association aborted by remote peer");
    }
    else
    {
      errorCond(cond, "DIMSE Failure (aborting association):");
      cond = ASC_abortAssociation(*assoc);
      errorCond(cond, "Cannot abort association:");
      if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusError, "DIMSE failure, aborting association");
    }
  }
  dropAssociation(assoc);
}