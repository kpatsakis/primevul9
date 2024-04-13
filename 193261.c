OFBool DcmSCP::isConnected() const
{
  return (m_assoc != NULL) && (m_assoc->DULassociation != NULL);
}