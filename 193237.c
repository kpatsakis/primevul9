void DcmSCP::refuseAssociation(const DcmRefuseReasonType reason)
{
  if (m_assoc == NULL)
  {
    DCMNET_WARN("DcmSCP::refuseAssociation() called but actually no association running, ignoring");
    return;
  }

  T_ASC_RejectParameters rej;

  // dump some information if required
  switch( reason )
  {
    case DCMSCP_TOO_MANY_ASSOCIATIONS:
      DCMNET_INFO("Refusing Association (too many associations)");
      break;
    case DCMSCP_CANNOT_FORK:
      DCMNET_INFO("Refusing Association (cannot fork)");
      break;
    case DCMSCP_BAD_APPLICATION_CONTEXT_NAME:
      DCMNET_INFO("Refusing Association (bad application context)");
      break;
    case DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED:
      DCMNET_INFO("Refusing Association (called AE title not recognized)");
      break;
    case DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED:
      DCMNET_INFO("Refusing Association (calling AE title not recognized)");
      break;
    case DCMSCP_FORCED:
      DCMNET_INFO("Refusing Association (forced via command line)");
      break;
    case DCMSCP_NO_IMPLEMENTATION_CLASS_UID:
      DCMNET_INFO("Refusing Association (no implementation class UID provided)");
      break;
    case DCMSCP_NO_PRESENTATION_CONTEXTS:
      DCMNET_INFO("Refusing Association (no acceptable presentation contexts)");
      break;
    case DCMSCP_INTERNAL_ERROR:
      DCMNET_INFO("Refusing Association (internal error)");
      break;
    default:
      DCMNET_INFO("Refusing Association (unknown reason)");
      break;
  }

  // Set some values in the reject message depending on the reason
  switch( reason )
  {
    case DCMSCP_TOO_MANY_ASSOCIATIONS:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
      break;
    case DCMSCP_CANNOT_FORK:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
      break;
    case DCMSCP_BAD_APPLICATION_CONTEXT_NAME:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
      break;
    case DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
      break;
    case DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
      break;
    case DCMSCP_FORCED:
    case DCMSCP_NO_IMPLEMENTATION_CLASS_UID:
    case DCMSCP_NO_PRESENTATION_CONTEXTS:
    case DCMSCP_INTERNAL_ERROR:
    default:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_NOREASON;
      break;
  }

  // Reject the association request.
  ASC_rejectAssociation( m_assoc, &rej );

  // Drop and destroy the association.
  dropAndDestroyAssociation();
}