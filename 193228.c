OFCondition WlmActivityManager::HandleFindSCP( T_ASC_Association *assoc, T_DIMSE_C_FindRQ *request, T_ASC_PresentationContextID presID )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function processes a DIMSE C-FIND-RQ commmand that was
//                received over the network connection.
// Parameters   : assoc    - [in] The association (network connection to another DICOM application).
//                request  - [in] The DIMSE C-FIND-RQ message that was received.
//                presID   - [in] The ID of the presentation context which was specified in the PDV
//                                which contained the DIMSE command.
// Return Value : OFCondition value denoting success or error.
{
  // Create callback data which needs to be passed to DIMSE_findProvider later.
  OFString temp_str;
  WlmFindContextType context;
  context.dataSource = dataSource;
  context.priorStatus = WLM_PENDING;
  ASC_getAPTitles( assoc->params, NULL, context.ourAETitle, NULL );
  context.opt_sleepDuringFind = opt_sleepDuringFind;

  // Dump some information if required.
  DCMWLM_INFO(DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING, NULL, presID));

  // Handle a C-FIND-Request on the provider side: receive the data set that represents the search mask
  // over the network, try to select corresponding records that match the search mask from some data source
  // (this is done whithin the callback function FindCallback() that will be passed) and send corresponding
  // C-FIND-RSP messages to the other DICOM application this application is connected with. In the end,
  // also send the C-FIND-RSP message that indicates that there are no more search results.
  OFCondition cond = DIMSE_findProvider( assoc, presID, request, FindCallback, &context, opt_blockMode, opt_dimse_timeout );
  if( cond.bad() )
    DCMWLM_ERROR("Find SCP Failed: " << DimseCondition::dump(temp_str, cond));

  // If option "--sleep-after" is set we need to sleep opt_sleepAfterFind
  // seconds after having processed one C-FIND-Request message.
  if( opt_sleepAfterFind > 0 )
  {
    DCMWLM_INFO("Sleeping (after find): " << opt_sleepAfterFind << " secs");
    OFStandard::sleep( (unsigned int)opt_sleepAfterFind );
  }

  // return result
  return cond;
}