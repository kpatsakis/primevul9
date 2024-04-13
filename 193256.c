OFCondition DcmSCP::handleSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
                                       const T_ASC_PresentationContextID presID,
                                       DcmDataset *&reqDataset)
{
  // First, receive the C-STORE request
  OFCondition cond = receiveSTORERequest(reqMessage, presID, reqDataset);

  if (cond.good())
  {
    // Then, check the request message and dataset and return an DIMSE status code
    const Uint16 rspStatusCode = checkSTORERequest(reqMessage, reqDataset);
    // ... that is sent back with the C-STORE response message
    cond = sendSTOREResponse(presID, reqMessage, rspStatusCode);
  }

  return cond;
}