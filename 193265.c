OFCondition DcmSCP::addPresentationContext(const OFString &abstractSyntax,
                                           const OFList<OFString> xferSyntaxes,
                                           const T_ASC_SC_ROLE role,
                                           const OFString &profile)
{
  return m_cfg->addPresentationContext(abstractSyntax,xferSyntaxes,role,profile);
}