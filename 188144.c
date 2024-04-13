MagickExport const DelegateInfo **GetDelegateInfoList(const char *pattern,
  size_t *number_delegates,ExceptionInfo *exception)
{
  const DelegateInfo
    **delegates;

  register const DelegateInfo
    *p;

  register ssize_t
    i;

  /*
    Allocate delegate list.
  */
  assert(number_delegates != (size_t *) NULL);
  assert(pattern != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",pattern);

  *number_delegates=0;
  p=GetDelegateInfo("*","*",exception);
  if (p == (const DelegateInfo *) NULL)
    return((const DelegateInfo **) NULL);
  delegates=(const DelegateInfo **) AcquireQuantumMemory((size_t)
    GetNumberOfElementsInLinkedList(delegate_cache)+1UL,sizeof(*delegates));
  if (delegates == (const DelegateInfo **) NULL)
    return((const DelegateInfo **) NULL);
  /*
    Generate delegate list.
  */
  LockSemaphoreInfo(delegate_semaphore);
  ResetLinkedListIterator(delegate_cache);
  p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
  for (i=0; p != (const DelegateInfo *) NULL; )
  {
    if( (p->stealth == MagickFalse) &&
        ( GlobExpression(p->decode,pattern,MagickFalse) != MagickFalse ||
          GlobExpression(p->encode,pattern,MagickFalse) != MagickFalse) )
      delegates[i++]=p;
    p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
  }
  UnlockSemaphoreInfo(delegate_semaphore);
  qsort((void *) delegates,(size_t) i,sizeof(*delegates),DelegateInfoCompare);
  delegates[i]=(DelegateInfo *) NULL;
  *number_delegates=(size_t) i;
  return(delegates);
}