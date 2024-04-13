OFCondition DcmSCP::processAssociationRQ()
{
  DcmSCPActionType desiredAction = DCMSCP_ACTION_UNDEFINED;
  if ( (m_assoc == NULL) || (m_assoc->params == NULL) )
    return ASC_NULLKEY;

  // call notifier function
  notifyAssociationRequest(*m_assoc->params, desiredAction);
  if (desiredAction != DCMSCP_ACTION_UNDEFINED)
  {
    if (desiredAction == DCMSCP_ACTION_REFUSE_ASSOCIATION)
    {
      refuseAssociation( DCMSCP_INTERNAL_ERROR );
      dropAndDestroyAssociation();
      return EC_Normal;
    }
    else desiredAction = DCMSCP_ACTION_UNDEFINED; // reset for later use
  }

  // Now we have to figure out if we might have to refuse the association request.
  // This is the case if at least one of five conditions is met:

  // Condition 1: if option "--refuse" is set we want to refuse the association request.
  if( m_cfg->getRefuseAssociation() )
  {
    refuseAssociation( DCMSCP_FORCED );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // Condition 2: determine the application context name. If an error occurred or if the
  // application context name is not supported we want to refuse the association request.
  char buf[BUFSIZ];
  OFCondition cond = ASC_getApplicationContextName( m_assoc->params, buf );
  if( cond.bad() || strcmp( buf, DICOM_STDAPPLICATIONCONTEXT ) != 0 )
  {
    refuseAssociation( DCMSCP_BAD_APPLICATION_CONTEXT_NAME );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // Condition 3: if the calling or called application entity title is not supported
  // we want to refuse the association request
  if (!checkCalledAETitleAccepted(m_assoc->params->DULparams.calledAPTitle))
  {
    refuseAssociation( DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  if (!checkCallingAETitleAccepted(m_assoc->params->DULparams.callingAPTitle))
  {
    refuseAssociation( DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  /* set our application entity title */
  if (m_cfg->getRespondWithCalledAETitle())
    ASC_setAPTitles(m_assoc->params, NULL, NULL, m_assoc->params->DULparams.calledAPTitle);
  else
    ASC_setAPTitles(m_assoc->params, NULL, NULL, m_cfg->getAETitle().c_str());

  /* If we get to this point the association shall be negotiated.
     Thus, for every presentation context it is checked whether
     it can be accepted. However, this is only a "dry" run, i.e.
     there is not yet sent a response message to the SCU
   */
  cond = negotiateAssociation();
  if( cond.bad() )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // Reject association if no presentation context was negotiated
  if( ASC_countAcceptedPresentationContexts( m_assoc->params ) == 0 )
  {
    // Dump some debug information
    OFString tempStr;
    DCMNET_INFO("No Acceptable Presentation Contexts");
    if (m_cfg->getVerbosePCMode())
      DCMNET_INFO(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ));
    else
      DCMNET_DEBUG(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ));
    refuseAssociation( DCMSCP_NO_PRESENTATION_CONTEXTS );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // If the negotiation was successful, accept the association request
  cond = ASC_acknowledgeAssociation( m_assoc );
  if( cond.bad() )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }
  notifyAssociationAcknowledge();

  // Dump some debug information
  OFString tempStr;
  DCMNET_INFO("Association Acknowledged (Max Send PDV: " << OFstatic_cast(Uint32, m_assoc->sendPDVLength) << ")");
  if (m_cfg->getVerbosePCMode())
    DCMNET_INFO(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC));
  else
    DCMNET_DEBUG(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC));

   // Go ahead and handle the association (i.e. handle the callers requests) in this process
   handleAssociation();
   return EC_Normal;
}