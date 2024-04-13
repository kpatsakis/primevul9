OFString DcmSCP::getCalledAETitle() const
{
  if (m_assoc == NULL)
    return "";
  return m_assoc->params->DULparams.calledAPTitle;
}