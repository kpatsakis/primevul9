Uint32 DcmSCP::getPeerMaxPDULength() const
{
  if (m_assoc == NULL)
    return 0;
  return m_assoc->params->theirMaxPDUReceiveSize;
}