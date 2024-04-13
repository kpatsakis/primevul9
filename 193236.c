OFCondition WlmActivityManager::ReceiveAndHandleCommands( T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling the other DICOM application's request.
// Parameters   : assoc - [in] The association (network connection to another DICOM application).
// Return Value : An OFCondition value 'cond' for which 'cond.bad()' will always be set
//                indicating that either some kind of error occurred, or that the peer aborted
//                the association (DUL_PEERABORTEDASSOCIATION), or that the peer requested the
//                release of the association (DUL_PEERREQUESTEDRELEASE).
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID;

  // Tell object that manages the data source if it should fail on an invalid query or not.
  dataSource->SetFailOnInvalidQuery( opt_failInvalidQuery );

  // start a loop to be able to receive more than one DIMSE command
  while( cond.good() )
  {
    // receive a DIMSE command over the network
    cond = DIMSE_receiveCommand( assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL );

    // check if peer did release or abort, or if we have a valid message
    if( cond.good() )
    {
      // in case we received a valid message, process this command
      // note that we can only process a C-ECHO-RQ, a C-FIND-RQ and a C-CANCEL-RQ
      switch( msg.CommandField )
      {
        case DIMSE_C_ECHO_RQ:
          // Process C-ECHO-Request
          cond = HandleEchoSCP( assoc, &msg.msg.CEchoRQ, presID );
          break;
        case DIMSE_C_FIND_RQ:
          // Process C-FIND-Request
          cond = HandleFindSCP( assoc, &msg.msg.CFindRQ, presID );
          break;
        case DIMSE_C_CANCEL_RQ:
          // Process C-CANCEL-Request
          // This is a late cancel request, just ignore it
          DCMWLM_WARN("Received late Cancel Request, ignoring");
          break;
        default:
          // We cannot handle this kind of message.
          // (Note that the condition will be returned and that the caller will abort the association.)
          cond = DIMSE_BADCOMMANDTYPE;
          break;
      }
    }
  }

  // return result
  return( cond );
}