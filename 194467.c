static LogInfo *GetLogInfo(const char *name,ExceptionInfo *exception)
{
  register LogInfo
    *p;

  assert(exception != (ExceptionInfo *) NULL);
  if (IsLogCacheInstantiated(exception) == MagickFalse)
    return((LogInfo *) NULL);
  /*
    Search for log tag.
  */
  LockSemaphoreInfo(log_semaphore);
  ResetLinkedListIterator(log_cache);
  p=(LogInfo *) GetNextValueInLinkedList(log_cache);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    {
      UnlockSemaphoreInfo(log_semaphore);
      return(p);
    }
  while (p != (LogInfo *) NULL)
  {
    if (LocaleCompare(name,p->name) == 0)
      break;
    p=(LogInfo *) GetNextValueInLinkedList(log_cache);
  }
  if (p != (LogInfo *) NULL)
    (void) InsertValueInLinkedList(log_cache,0,
      RemoveElementByValueFromLinkedList(log_cache,p));
  UnlockSemaphoreInfo(log_semaphore);
  return(p);
}