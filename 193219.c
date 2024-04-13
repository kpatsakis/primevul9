void DcmSCP::dropAndDestroyAssociation()
{
  if (m_assoc)
  {
    notifyAssociationTermination();
    ASC_dropSCPAssociation( m_assoc );
    ASC_destroyAssociation( &m_assoc );
  }
}