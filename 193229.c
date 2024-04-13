OFCondition DcmSCP::handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                          const DcmPresentationContextInfo &info)
{
  OFCondition cond;
  if( incomingMsg->CommandField == DIMSE_C_ECHO_RQ )
  {
    // Process C-ECHO request
    cond = handleECHORequest( incomingMsg->msg.CEchoRQ, info.presentationContextID );
  } else {
    // We cannot handle this kind of message. Note that the condition will be returned
    // and that the caller is responsible to end the association if desired.
    OFString tempStr;
    DCMNET_ERROR("Cannot handle this kind of DIMSE command (0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << OFstatic_cast(unsigned int, incomingMsg->CommandField) << ")");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, *incomingMsg, DIMSE_INCOMING));
    cond = DIMSE_BADCOMMANDTYPE;
  }

  // return result
  return cond;
}