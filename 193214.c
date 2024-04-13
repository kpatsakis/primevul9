static OFCondition acceptUnknownContextsWithPreferredTransferSyntaxes(
  T_ASC_Parameters * params,
  const char* transferSyntaxes[], int transferSyntaxCount,
  T_ASC_SC_ROLE acceptedRole)
{
  OFCondition cond = EC_Normal;
  /*
  ** Accept in the order "least wanted" to "most wanted" transfer
  ** syntax.  Accepting a transfer syntax will override previously
  ** accepted transfer syntaxes.
  */
  for (int i = transferSyntaxCount - 1; i >= 0; i--)
  {
    cond = acceptUnknownContextsWithTransferSyntax(params, transferSyntaxes[i], acceptedRole);
    if (cond.bad()) return cond;
  }
  return cond;
}