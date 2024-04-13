Uint16 DcmSCP::checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ & /*reqMessage*/,
                                       DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}