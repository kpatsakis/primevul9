static void dropAssociation(T_ASC_Association **assoc)
{
  if ((assoc == NULL)||(*assoc == NULL)) return;
  OFCondition cond = ASC_dropSCPAssociation(*assoc);
  errorCond(cond, "Cannot Drop Association:");
  cond = ASC_destroyAssociation(assoc);
  errorCond(cond, "Cannot Destroy Association:");
  *assoc = NULL;
  return;
}