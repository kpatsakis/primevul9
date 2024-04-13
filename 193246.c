storeProgressCallback(
    /* in */
    void *callbackData,
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName,                /* being received into */
    DcmDataset **imageDataSet,          /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
  if (progress->state == DIMSE_StoreEnd)
  {
    StoreContext *context = (StoreContext *)callbackData;
    if (rsp->DimseStatus == STATUS_Success)
    {
      if ((imageDataSet)&&(*imageDataSet))
      {
        checkRequestAgainstDataset(req, NULL, *imageDataSet, rsp, context->opt_correctUIDPadding);
      } else {
        checkRequestAgainstDataset(req, imageFileName, NULL, rsp, context->opt_correctUIDPadding);
      }
    }

    if (rsp->DimseStatus == STATUS_Success)
    {
      if ((imageDataSet)&&(*imageDataSet))
      {
        OFCondition cond = context->dcmff->saveFile(context->fileName, EXS_LittleEndianExplicit,
          EET_ExplicitLength, EGL_recalcGL);
        if (! cond.good())
        {
          OFLOG_ERROR(dcmpsrcvLogger, "Cannot write image file: " << context->fileName);
          rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        }
      }
      saveImageToDB(context, req, context->fileName, rsp, statusDetail);
    }
  }
}