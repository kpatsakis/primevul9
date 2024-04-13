OFCondition DcmSCP::checkForCANCEL(T_ASC_PresentationContextID presID,
                                   const Uint16 messageID)
{
  return DIMSE_checkForCancelRQ(m_assoc, presID, messageID);
}