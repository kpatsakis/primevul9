OFCondition DcmSCP::negotiateAssociation()
{
  // check whether there is something to negotiate...
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  /* set presentation contexts as defined in association configuration */
  OFCondition result = m_cfg->evaluateIncomingAssociation(*m_assoc);
  if (result.bad())
  {
    OFString tempStr;
    DCMNET_ERROR(DimseCondition::dump(tempStr, result));
  }
  return result;
}