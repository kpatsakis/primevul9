OFString DcmSCP::getPeerIP() const
{
  if (m_assoc == NULL)
    return "";
  return m_assoc->params->DULparams.callingPresentationAddress;
}