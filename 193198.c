OFCondition WlmActivityManager::HandleEchoSCP( T_ASC_Association *assoc, T_DIMSE_C_EchoRQ *req, T_ASC_PresentationContextID presId )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Having received a DIMSE C-ECHO-RQ message, this function takes care of sending a
//                DIMSE C-ECHO-RSP message over the network connection.
// Parameters   : assoc   - [in] The association (network connection to another DICOM application).
//                request - [in] The DIMSE C-ECHO-RQ message that was received.
//                presID  - [in] The ID of the presentation context which was specified in the PDV
//                               which contained the DIMSE command.
// Return Value : OFCondition value denoting success or error.
{
  OFString temp_str;

  // Dump information if required
  DCMWLM_INFO("Received Echo Request");
  DCMWLM_DEBUG(DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presId));

  // Send an echo response
  OFCondition cond = DIMSE_sendEchoResponse( assoc, presId, req, STATUS_Success, NULL );
  if( cond.bad() )
    DCMWLM_ERROR("Echo SCP Failed: " << DimseCondition::dump(temp_str, cond));

  // return return value
  return cond;
}