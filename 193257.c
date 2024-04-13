void DcmSCP::handleAssociation()
{
  if (m_assoc == NULL)
  {
    DCMNET_WARN("DcmSCP::handleAssociation() called but SCP actually has no association running, ignoring");
    return;
  }

  // Receive a DIMSE command and perform all the necessary actions. (Note that ReceiveAndHandleCommands()
  // will always return a value 'cond' for which 'cond.bad()' will be true. This value indicates that either
  // some kind of error occurred, or that the peer aborted the association (DUL_PEERABORTEDASSOCIATION),
  // or that the peer requested the release of the association (DUL_PEERREQUESTEDRELEASE).) (Also note
  // that ReceiveAndHandleCommands() will never return EC_Normal.)
  OFCondition cond = EC_Normal;
  T_DIMSE_Message message;
  T_ASC_PresentationContextID presID;

  // start a loop to be able to receive more than one DIMSE command
  while( cond.good() )
  {
    // receive a DIMSE command over the network
    cond = DIMSE_receiveCommand( m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                 &presID, &message, NULL );

    // check if peer did release or abort, or if we have a valid message
    if( cond.good() )
    {
      DcmPresentationContextInfo pcInfo;
      getPresentationContextInfo(m_assoc, presID, pcInfo);
      cond = handleIncomingCommand(&message, pcInfo);
    }
  }
  // Clean up on association termination.
  if( cond == DUL_PEERREQUESTEDRELEASE )
  {
    notifyReleaseRequest();
    ASC_acknowledgeRelease(m_assoc);
  }
  else if( cond == DUL_PEERABORTEDASSOCIATION )
  {
    notifyAbortRequest();
  }
  else
  {
    notifyDIMSEError(cond);
    ASC_abortAssociation( m_assoc );
  }

  // Drop and destroy the association.
  dropAndDestroyAssociation();

  // Output separator line.
  DCMNET_DEBUG( "+++++++++++++++++++++++++++++" );
}