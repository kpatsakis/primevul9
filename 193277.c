checkRequestAgainstDataset(
    T_DIMSE_C_StoreRQ *req,     /* original store request */
    const char* fname,          /* filename of dataset */
    DcmDataset *dataSet,        /* dataset to check */
    T_DIMSE_C_StoreRSP *rsp,    /* final store response */
    OFBool opt_correctUIDPadding)
{
    DcmFileFormat ff;
    if (dataSet == NULL)
    {
      /* load the data from file */
      if (ff.loadFile(fname).bad())
      {
        OFLOG_ERROR(dcmpsrcvLogger, "Cannot open file: " << fname);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
      }
      dataSet = ff.getDataset();
    }

    /* which SOP class and SOP instance ? */
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if (!DU_findSOPClassAndInstanceInDataSet(dataSet, sopClass, sopInstance, opt_correctUIDPadding))
    {
      OFLOG_ERROR(dcmpsrcvLogger, "Bad image file: " << fname);
      rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
    }
    else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
    {
      rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    }
    else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
    {
      rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    }
    else if (strcmp(sopClass, UID_GrayscaleSoftcopyPresentationStateStorage) == 0)
    {
      /* we have received a presentation state. Check if we can parse it! */
      DcmPresentationState pstate;
      if (EC_Normal != pstate.read(*dataSet))
      {
        OFLOG_ERROR(dcmpsrcvLogger, "Grayscale softcopy presentation state object cannot be displayed - rejected");
        rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
      }
    }
    return;
}