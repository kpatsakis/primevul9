MagickExport char **GetDelegateList(const char *pattern,
  size_t *number_delegates,ExceptionInfo *exception)
{
  char
    **delegates;

  register const DelegateInfo
    *p;

  register ssize_t
    i;

  /*
    Allocate delegate list.
  */
  assert(pattern != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",pattern);

  assert(number_delegates != (size_t *) NULL);
  *number_delegates=0;
  p=GetDelegateInfo("*","*",exception);
  if (p == (const DelegateInfo *) NULL)
    return((char **) NULL);
  delegates=(char **) AcquireQuantumMemory((size_t)
    GetNumberOfElementsInLinkedList(delegate_cache)+1UL,sizeof(*delegates));
  if (delegates == (char **) NULL)
    return((char **) NULL);
  LockSemaphoreInfo(delegate_semaphore);
  ResetLinkedListIterator(delegate_cache);
  p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
  for (i=0; p != (const DelegateInfo *) NULL; )
  {
    if( (p->stealth == MagickFalse) &&
        GlobExpression(p->decode,pattern,MagickFalse) != MagickFalse )
      delegates[i++]=ConstantString(p->decode);
    if( (p->stealth == MagickFalse) &&
        GlobExpression(p->encode,pattern,MagickFalse) != MagickFalse )
      delegates[i++]=ConstantString(p->encode);
    p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
  }
  UnlockSemaphoreInfo(delegate_semaphore);
  qsort((void *) delegates,(size_t) i,sizeof(*delegates),DelegateCompare);
  delegates[i]=(char *) NULL;
  *number_delegates=(size_t) i;
  return(delegates);
}