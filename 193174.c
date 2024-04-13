Uint16 DcmSCP::checkSTORERequest(T_DIMSE_C_StoreRQ & /*reqMessage*/,
                                 DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}