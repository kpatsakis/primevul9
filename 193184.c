void WlmActivityManager::RefuseAssociation( T_ASC_Association **assoc, WlmRefuseReasonType reason )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of refusing an assocation request.
// Parameters   : assoc  - [in] The association (network connection to another DICOM application).
//                reason - [in] The reason why the association request will be refused.
// Return Value : none.
{
  T_ASC_RejectParameters rej;

  // Dump some information if required.
  switch( reason )
  {
    case WLM_TOO_MANY_ASSOCIATIONS: DCMWLM_INFO("Refusing Association (too many associations)"); break;
    case WLM_CANNOT_FORK:           DCMWLM_INFO("Refusing Association (cannot fork)"); break;
    case WLM_BAD_APP_CONTEXT:       DCMWLM_INFO("Refusing Association (bad application context)"); break;
    case WLM_BAD_AE_SERVICE:        DCMWLM_INFO("Refusing Association (bad application entity service)"); break;
    case WLM_FORCED:                DCMWLM_INFO("Refusing Association (forced via command line)"); break;
    case WLM_NO_IC_UID:             DCMWLM_INFO("Refusing Association (no implementation class UID provided)"); break;
    default:                        DCMWLM_INFO("Refusing Association (unknown reason)"); break;
  }

  // Set some values in the reject message depending on the reason.
  switch( reason )
  {
    case WLM_TOO_MANY_ASSOCIATIONS:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
      break;
    case WLM_CANNOT_FORK:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
      break;
    case WLM_BAD_APP_CONTEXT:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
      break;
    case WLM_BAD_AE_SERVICE:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
      break;
    case WLM_FORCED:
    case WLM_NO_IC_UID:
    default:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_NOREASON;
      break;
  }

  // Reject the association request.
  ASC_rejectAssociation( *assoc, &rej );

  // Drop the association.
  ASC_dropAssociation( *assoc );

  // Destroy the association.
  ASC_destroyAssociation( assoc );
}