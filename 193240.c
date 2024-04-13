OFCondition DcmSCP::waitForAssociationRQ(T_ASC_Network *network)
{
  if (network == NULL)
    return ASC_NULLKEY;
  if (m_assoc != NULL)
    return DIMSE_ILLEGALASSOCIATION;

  Uint32 timeout = m_cfg->getConnnectionTimeout();

  // Listen to a socket for timeout seconds and wait for an association request
  OFCondition cond = ASC_receiveAssociation( network, &m_assoc, m_cfg->getMaxReceivePDULength(), NULL, NULL, OFFalse,
                                             m_cfg->getConnectionBlockingMode(), OFstatic_cast(int, timeout) );

  // just return, if timeout occurred (DUL_NOASSOCIATIONREQUEST)
  if ( cond == DUL_NOASSOCIATIONREQUEST )
  {
    return EC_Normal;
  }

  // if error occurs close association and return
  if( cond.bad() )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  return processAssociationRQ();
}