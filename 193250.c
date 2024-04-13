static associationType negotiateAssociation(
  T_ASC_Network *net,
  T_ASC_Association **assoc,
  const char *aetitle,
  unsigned long maxPDU,
  OFBool opt_networkImplicitVROnly,
  OFBool useTLS)
{
    associationType result = assoc_success;
    char buf[BUFSIZ];
    OFBool dropAssoc = OFFalse;

    OFCondition cond = ASC_receiveAssociation(net, assoc, maxPDU, NULL, NULL, useTLS);

    if (errorCond(cond, "Failed to receive association:"))
    {
      dropAssoc = OFTrue;
      result = assoc_error;

      if (messageClient)
      {
        // notify about failed association setup
        OFOStringStream out;
        out << "Unable to Receive DIMSE Association Request:" << OFendl << cond.text() << OFendl << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(out, theString)
        if (useTLS)
          messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
          else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
        OFSTRINGSTREAM_FREESTR(theString)
      }
    }
    else
    {
      OFLOG_INFO(dcmpsrcvLogger, "Association Received ("
          << (*assoc)->params->DULparams.callingPresentationAddress << ":"
          << (*assoc)->params->DULparams.callingAPTitle << " -> "
          << (*assoc)->params->DULparams.calledAPTitle << ")");

      ASC_setAPTitles((*assoc)->params, NULL, NULL, aetitle);
      /* Application Context Name */
      cond = ASC_getApplicationContextName((*assoc)->params, buf);
      if (cond.bad() || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
      {
          /* reject: the application context name is not supported */
          OFLOG_INFO(dcmpsrcvLogger, "Bad AppContextName: " << buf);
          cond = refuseAssociation(*assoc, ref_BadAppContext);

          if (messageClient)
          {
            // notify about rejected association
            OFOStringStream out;
            OFString temp_str;
            out << "DIMSE Association Rejected:" << OFendl
                << "  reason: bad application context name '" << buf << "'" << OFendl
                << "  calling presentation address: " << (*assoc)->params->DULparams.callingPresentationAddress << OFendl
                << "  calling AE title: " << (*assoc)->params->DULparams.callingAPTitle << OFendl
                << "  called AE title: " << (*assoc)->params->DULparams.calledAPTitle << OFendl;
            out << ASC_dumpConnectionParameters(temp_str, *assoc) << OFendl;
            out << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(out, theString)
            if (useTLS)
              messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
              else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
            OFSTRINGSTREAM_FREESTR(theString)
          }
          dropAssoc = OFTrue;
          result = assoc_error;
      } else {

        const char *nonStorageSyntaxes[] =
        {
          UID_VerificationSOPClass,
          UID_PrivateShutdownSOPClass
        };

        const char* transferSyntaxes[] = { NULL, NULL, NULL };
        int numTransferSyntaxes = 0;
        if (opt_networkImplicitVROnly)
        {
          transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
          numTransferSyntaxes = 1;
        } else {
          transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
          numTransferSyntaxes = 3;

          if (gLocalByteOrder == EBO_LittleEndian) {
            /* we are on a little endian machine */
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
          } else {
            /* we are on a big endian machine */
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
          }
        }

        /*  accept any of the non-storage syntaxes */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          (*assoc)->params,
          (const char**)nonStorageSyntaxes, DIM_OF(nonStorageSyntaxes),
          (const char**)transferSyntaxes, numTransferSyntaxes);
        errorCond(cond, "Cannot accept presentation contexts:");

        /*  accept any of the storage syntaxes */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          (*assoc)->params,
          dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,
          (const char**)transferSyntaxes, numTransferSyntaxes);
        errorCond(cond, "Cannot accept presentation contexts:");
      }

      /* check if we have negotiated the private "shutdown" SOP Class */
      if (0 != ASC_findAcceptedPresentationContextID(*assoc, UID_PrivateShutdownSOPClass))
      {
        // we don't notify the IPC server about this incoming connection
        cond = refuseAssociation(*assoc, ref_NoReason);
        dropAssoc = OFTrue;
        result = assoc_terminate;
      }
    } /* receiveAssociation successful */

    if (dropAssoc) dropAssociation(assoc);
    return result;
}