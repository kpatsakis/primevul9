static void FindCallback( void *callbackData, OFBool cancelled, T_DIMSE_C_FindRQ * /*request*/, DcmDataset *requestIdentifiers, int responseCount, T_DIMSE_C_FindRSP *response, DcmDataset **responseIdentifiers, DcmDataset **statusDetail )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function will try to select another record from a database which matches the
//                search mask that was passed. In certain circumstances, the selected information
//                will be dumped to stdout.
// Parameters   : callbackData        - [in] data for this callback function
//                cancelled           - [in] Specifies if we encounteres a C-CANCEL-RQ. In such a case
//                                      the search shall be cancelled.
//                request             - [in] The original C-FIND-RQ message.
//                requestIdentifiers  - [in] Contains the search mask.
//                responseCount       - [in] If we labelled C-FIND-RSP messages consecutively, starting
//                                      at label "1", this number would provide the label's number.
//                response            - [inout] the C-FIND-RSP message (will be sent after the call to
//                                      this function). The status field will be set in this function.
//                responseIdentifiers - [inout] Will in the end contain the next record that matches the
//                                      search mask (captured in requestIdentifiers)
//                statusDetail        - [inout] This variable can be used to capture detailed information
//                                      with regard to the status information which is captured in the
//                                      status element (0000,0900) of the C-FIND-RSP message.
// Return Value : OFCondition value denoting success or error.
{
  WlmDataSourceStatusType dbstatus;
  WlmFindContextType *context = NULL;
  WlmDataSource *dataSource = NULL;
  OFCmdUnsignedInt opt_sleepDuringFind = 0;

  // Recover contents of context.
  context = (WlmFindContextType*)callbackData;
  dataSource = context->dataSource;
  opt_sleepDuringFind = context->opt_sleepDuringFind;

  // Determine the data source's current status.
  dbstatus = context->priorStatus;

  // If this is the first time this callback function is called, we need to do something special
  if( responseCount == 1 )
  {
    // Dump some information if required
    DCMWLM_INFO("Find SCP Request Identifiers:" << OFendl
      << DcmObject::PrintHelper(*requestIdentifiers) << OFendl
      << "=============================");

    // Determine the records that match the search mask. After this call, the
    // matching records will be available through dataSource->nextFindResponse(...).)
    dbstatus = dataSource->StartFindRequest( *requestIdentifiers );
    if( !( dbstatus == WLM_PENDING || dbstatus == WLM_PENDING_WARNING || dbstatus == WLM_SUCCESS) )
      DCMWLM_DEBUG("Worklist Database: StartFindRequest() Failed (" << DU_cfindStatusString((Uint16)dbstatus) << ")");

    DCMWLM_INFO("=============================");
  }

  // If opt_sleepDuringFind is set the application is supposed
  // to sleep n seconds during the find process.
  if( opt_sleepDuringFind > 0 )
  {
    DCMWLM_INFO("SLEEPING (during find): " << opt_sleepDuringFind << " secs");
    OFStandard::sleep((unsigned int)opt_sleepDuringFind);
  }

  // If we encountered a C-CANCEL-RQ and if we have pending
  // responses, the search shall be cancelled
  if( cancelled && ( dbstatus == WLM_PENDING || dbstatus == WLM_PENDING_WARNING ) )
    dbstatus = dataSource->CancelFindRequest();

  // If the dbstatus is "pending" try to select another matching record.
  if( dbstatus == WLM_PENDING || dbstatus == WLM_PENDING_WARNING )
  {
    // Get the next matching record/data set
    *responseIdentifiers = dataSource->NextFindResponse( dbstatus );
  }

  // Dump some information if required
  if (DCM_dcmwlmLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
  {
    DCMWLM_INFO("Worklist Find SCP Response " << responseCount << " (" << DU_cfindStatusString((Uint16)dbstatus) << ")");
    if( *responseIdentifiers != NULL && (*responseIdentifiers)->card() > 0 )
    {
      DCMWLM_INFO(DcmObject::PrintHelper(**responseIdentifiers) << OFendl << "-----------------------------");
    }
  }

  // Set response status
  response->DimseStatus = dbstatus;

  // Delete status detail information if there is some
  if( *statusDetail != NULL )
  {
    delete *statusDetail;
    *statusDetail = NULL;
  }

  // Depending on the data source's current status, we may have to
  // return status detail information.
  switch( dbstatus )
  {
    case WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS:
    case WLM_FAILED_UNABLE_TO_PROCESS:
      DCMWLM_WARN(AddStatusDetail( statusDetail, dataSource->GetOffendingElements()));
      DCMWLM_WARN(AddStatusDetail( statusDetail, dataSource->GetErrorComments()));
      break;
    case WLM_REFUSED_OUT_OF_RESOURCES:
      // out of resources may only have error comment detail
      DCMWLM_WARN(AddStatusDetail( statusDetail, dataSource->GetErrorComments()));
      break;
    default:
      // other status codes may not have any status detail
      break;
  }
}