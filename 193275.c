subOpSCP(T_ASC_Association **subAssoc)
{
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;

    if (!ASC_dataWaiting(*subAssoc, 0)) /* just in case */
        return DIMSE_NODATAAVAILABLE;

    OFCondition cond = DIMSE_receiveCommand(*subAssoc, opt_blockMode, opt_dimse_timeout, &presID, &msg, NULL);

    if (cond == EC_Normal) {
      switch (msg.CommandField)
      {
        case DIMSE_C_ECHO_RQ:
          // process C-ECHO-Request
          cond = echoSCP(*subAssoc, &msg, presID);
          break;
        case DIMSE_C_STORE_RQ:
          // process C-STORE-Request
          cond = storeSCP(*subAssoc, &msg, presID);
          break;
        default:
          OFString tempStr;
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          OFLOG_ERROR(movescuLogger, "Expected C-ECHO or C-STORE request but received DIMSE command 0x"
              << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
              << OFstatic_cast(unsigned, msg.CommandField));
          OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(tempStr, msg, DIMSE_INCOMING, NULL, presID));
          break;
      }
    }
    /* clean up on association termination */
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        cond = ASC_acknowledgeRelease(*subAssoc);
        ASC_dropSCPAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
        return cond;
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
    }
    else if (cond != EC_Normal)
    {
        OFString temp_str;
        OFLOG_ERROR(movescuLogger, "DIMSE failure (aborting sub-association): " << DimseCondition::dump(temp_str, cond));
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation(*subAssoc);
    }

    if (cond != EC_Normal)
    {
        ASC_dropAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
    }
    return cond;
}