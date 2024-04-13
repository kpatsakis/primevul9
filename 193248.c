DUL_PRESENTATIONCONTEXT* DcmSCP::findPresentationContextID(LST_HEAD *head,
                                                           T_ASC_PresentationContextID presentationContextID)
{
  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (head == NULL)
    return NULL;

  l = &head;
  if (*l == NULL)
    return NULL;

  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);

  while (pc && !found)
  {
    if (pc->presentationContextID == presentationContextID)
    {
      found = OFTrue;
    } else
    {
      pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
  }
  return pc;
}