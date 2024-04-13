MagickExport const DelegateInfo *GetDelegateInfo(const char *decode,
  const char *encode,ExceptionInfo *exception)
{
  register const DelegateInfo
    *p;

  assert(exception != (ExceptionInfo *) NULL);
  if (IsDelegateCacheInstantiated(exception) == MagickFalse)
    return((const DelegateInfo *) NULL);
  /*
    Search for named delegate.
  */
  LockSemaphoreInfo(delegate_semaphore);
  ResetLinkedListIterator(delegate_cache);
  p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
  if ((LocaleCompare(decode,"*") == 0) && (LocaleCompare(encode,"*") == 0))
    {
      UnlockSemaphoreInfo(delegate_semaphore);
      return(p);
    }
  while (p != (const DelegateInfo *) NULL)
  {
    if (p->mode > 0)
      {
        if (LocaleCompare(p->decode,decode) == 0)
          break;
        p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
        continue;
      }
    if (p->mode < 0)
      {
        if (LocaleCompare(p->encode,encode) == 0)
          break;
        p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
        continue;
      }
    if (LocaleCompare(decode,p->decode) == 0)
      if (LocaleCompare(encode,p->encode) == 0)
        break;
    if (LocaleCompare(decode,"*") == 0)
      if (LocaleCompare(encode,p->encode) == 0)
        break;
    if (LocaleCompare(decode,p->decode) == 0)
      if (LocaleCompare(encode,"*") == 0)
        break;
    p=(const DelegateInfo *) GetNextValueInLinkedList(delegate_cache);
  }
  if (p != (const DelegateInfo *) NULL)
    (void) InsertValueInLinkedList(delegate_cache,0,
      RemoveElementByValueFromLinkedList(delegate_cache,p));
  UnlockSemaphoreInfo(delegate_semaphore);
  return(p);
}