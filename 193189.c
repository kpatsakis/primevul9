OFCondition WlmActivityManager::WaitForAssociation( T_ASC_Network * net )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of receiving, negotiating and accepting/refusing an
//                association request. Additionally, it handles the request the association
//                requesting application transmits after a connection isd established.
// Parameters   : net - [in] Contains network parameters.
// Return Value : Indicator which shows if function was executed successfully.
{
  T_ASC_Association *assoc = NULL;
  char buf[BUFSIZ];
  int timeout;

  // Depending on if the execution is limited to one single process
  // or not we need to set the timeout value correspondingly.
  // for WIN32, child processes cannot be counted (always 0) -> timeout=1000
  if( opt_singleProcess )
    timeout = 1000;
  else
  {
    if( processTable.size() > 0 )
      timeout = 1;
    else
      timeout = 1000;
  }

  // Listen to a socket for timeout seconds and wait for an association request.
  OFCondition cond = ASC_receiveAssociation( net, &assoc, opt_maxPDU, NULL, NULL, OFFalse, DUL_NOBLOCK, timeout );

  // just return, if timeout occured (DUL_NOASSOCIATIONREQUEST)
  // or (WIN32) if dcmnet has started a child for us, to handle this
  // association (signaled by "DULC_FORKEDCHILD") -> return to "event loop"
  if ( ( cond.code() == DULC_FORKEDCHILD ) || ( cond == DUL_NOASSOCIATIONREQUEST ) )
    return EC_Normal;

  // if error occurs and we're not in single process mode, close association and return
  if( cond.bad() && !opt_singleProcess )
  {
    ASC_dropAssociation( assoc );
    ASC_destroyAssociation( &assoc );
    return EC_Normal;
  }
  // Dump some information if required
  DCMWLM_INFO("Association Received ("
    << assoc->params->DULparams.callingPresentationAddress
    << ":" << assoc->params->DULparams.callingAPTitle
    << " -> " << assoc->params->DULparams.calledAPTitle << ")");

  // Dump more information if required
  OFString temp_str;
  DCMWLM_DEBUG("Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));

  // Now we have to figure out if we might have to refuse the association request.
  // This is the case if at least one of five conditions is met:

  // Condition 1: if option "--refuse" is set we want to refuse the association request.
  if( opt_refuseAssociation )
  {
    RefuseAssociation( &assoc, WLM_FORCED );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 2: determine the application context name. If an error occurred or if the
  // application context name is not supported we want to refuse the association request.
  cond = ASC_getApplicationContextName( assoc->params, buf );
  if( cond.bad() || strcmp( buf, DICOM_STDAPPLICATIONCONTEXT ) != 0 )
  {
    RefuseAssociation( &assoc, WLM_BAD_APP_CONTEXT );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 3: if option "--reject" is set and the caller did not provide an
  // implementation class UID we want to refuse the association request
  if( opt_rejectWithoutImplementationUID && strlen( assoc->params->theirImplementationClassUID ) == 0 )
  {
    RefuseAssociation( &assoc, WLM_NO_IC_UID );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 4: if there are too many concurrent associations
  // we want to refuse the association request
  if( (int)processTable.size() >= opt_maxAssociations )
  {
    RefuseAssociation( &assoc, WLM_TOO_MANY_ASSOCIATIONS );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 5: if the called application entity title is not supported
  // whithin the data source we want to refuse the association request
  dataSource->SetCalledApplicationEntityTitle( assoc->params->DULparams.calledAPTitle );
  if( !dataSource->IsCalledApplicationEntityTitleSupported() )
  {
    RefuseAssociation( &assoc, WLM_BAD_AE_SERVICE );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // If we get to this point the association shall be negotiated.
  cond = NegotiateAssociation( assoc );
  if( cond.bad() )
  {
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Reject association if no presentation context was negotiated
  if( ASC_countAcceptedPresentationContexts( assoc->params ) == 0 )
  {
    DCMWLM_INFO("No Acceptable Presentation Contexts");
    RefuseAssociation( &assoc, WLM_FORCED );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // If the negotiation was successful, accept the association request.
  cond = ASC_acknowledgeAssociation( assoc );
  if( cond.bad() )
  {
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Dump some information if required.
  DCMWLM_INFO("Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")");

  // Dump some more information if required.
  DCMWLM_DEBUG(ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));

  // Depending on if this execution shall be limited to one process or not, spawn a sub-
  // process to handle the association or don't. (Note: For windows dcmnet is handling
  // the creation for a new subprocess, so we can call HandleAssociation directly, too)
  if( opt_singleProcess || opt_forkedChild )
  {
    // Go ahead and handle the association (i.e. handle the callers requests) in this process.
    HandleAssociation( assoc );
  }
#ifdef HAVE_FORK
  else
  {
    // Spawn a sub-process to handle the association (i.e. handle the callers requests)
    int pid = (int)(fork());
    if( pid < 0 )
    {
      RefuseAssociation( &assoc, WLM_CANNOT_FORK );
      if( !opt_singleProcess )
      {
        ASC_dropAssociation( assoc );
        ASC_destroyAssociation( &assoc );
      }
      return( EC_Normal );
    }
    else if( pid > 0 )
    {
      // Fork returns a positive process id if this is the parent process.
      // If this is the case, remeber the process in a table and go ahead.
      AddProcessToTable( pid, assoc );

      // the child will handle the association, we can drop it
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    else
    {
      // If the process id is not positive, this must be the child process.
      // We want to handle the association, i.e. the callers requests.
      HandleAssociation( assoc );

      // When everything is finished, terminate the child process.
      exit(0);
    }
  }
#endif // HAVE_FORK

  return( EC_Normal );
}