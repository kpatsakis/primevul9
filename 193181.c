OFCondition DcmSCP::sendSTOREResponse(T_ASC_PresentationContextID presID,
                                      const T_DIMSE_C_StoreRQ &reqMessage,
                                      const Uint16 rspStatusCode)
{
  // Call the method doing the real work
  return sendSTOREResponse(presID, reqMessage.MessageID, reqMessage.AffectedSOPClassUID, reqMessage.AffectedSOPInstanceUID,
    rspStatusCode, NULL /* statusDetail */);
}