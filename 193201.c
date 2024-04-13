void WlmActivityManager::HandleAssociation( T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling the other DICOM application's request. After
//                having accomplished all necessary steps, the association will be dropped and destroyed.
// Parameters   : assoc - [in] The association (network connection to another DICOM application).
// Return Value : none.
{
  // Receive a DIMSE command and perform all the necessary actions. (Note that ReceiveAndHandleCommands()
  // will always return a value 'cond' for which 'cond.bad()' will be true. This value indicates that either
  // some kind of error occurred, or that the peer aborted the association (DUL_PEERABORTEDASSOCIATION),
  // or that the peer requested the release of the association (DUL_PEERREQUESTEDRELEASE).) (Also note
  // that ReceiveAndHandleCommands() will never return EC_Normal.)
  OFCondition cond = ReceiveAndHandleCommands( assoc );

  // Clean up on association termination.
  if( cond == DUL_PEERREQUESTEDRELEASE )
  {
    DCMWLM_INFO("Association Release");
    ASC_acknowledgeRelease( assoc );
    ASC_dropSCPAssociation( assoc );
  }
  else if( cond == DUL_PEERABORTEDASSOCIATION )
  {
    DCMWLM_INFO("Association Aborted");
  }
  else
  {
    OFString temp_str;
    DCMWLM_ERROR("DIMSE failure (aborting association): " << DimseCondition::dump(temp_str, cond));
    ASC_abortAssociation( assoc );
  }

  // Drop and destroy the association.
  ASC_dropAssociation( assoc );
  ASC_destroyAssociation( &assoc );

  // Dump some information if required.
  DCMWLM_INFO("+++++++++++++++++++++++++++++");
}