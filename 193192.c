saveImageToDB(
    StoreContext *context,
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    const char *imageFileName,
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);

    /* Store image */
    if (context->status == STATUS_Success)
    {
      if (context->dbHandle->storeRequest(req->AffectedSOPClassUID, req->AffectedSOPInstanceUID,
          imageFileName, &dbStatus).bad())
      {
        OFLOG_ERROR(dcmpsrcvLogger, "storeSCP: Database: DB_storeRequest Failed ("
             << DU_cstoreStatusString(dbStatus.status()) << ")");
      }
      context->status = dbStatus.status();
    }

     rsp->DimseStatus = context->status;
    *statusDetail = dbStatus.extractStatusDetail();
    return;
}