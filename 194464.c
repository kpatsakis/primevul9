MagickExport char **GetLogList(const char *pattern,size_t *number_preferences,
  ExceptionInfo *exception)
{
  char
    **preferences;

  register const LogInfo
    *p;

  register ssize_t
    i;

  /*
    Allocate log list.
  */
  assert(pattern != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",pattern);
  assert(number_preferences != (size_t *) NULL);
  *number_preferences=0;
  p=GetLogInfo("*",exception);
  if (p == (const LogInfo *) NULL)
    return((char **) NULL);
  preferences=(char **) AcquireQuantumMemory((size_t)
    GetNumberOfElementsInLinkedList(log_cache)+1UL,sizeof(*preferences));
  if (preferences == (char **) NULL)
    return((char **) NULL);
  /*
    Generate log list.
  */
  LockSemaphoreInfo(log_semaphore);
  ResetLinkedListIterator(log_cache);
  p=(const LogInfo *) GetNextValueInLinkedList(log_cache);
  for (i=0; p != (const LogInfo *) NULL; )
  {
    if ((p->stealth == MagickFalse) &&
        (GlobExpression(p->name,pattern,MagickFalse) != MagickFalse))
      preferences[i++]=ConstantString(p->name);
    p=(const LogInfo *) GetNextValueInLinkedList(log_cache);
  }
  UnlockSemaphoreInfo(log_semaphore);
  qsort((void *) preferences,(size_t) i,sizeof(*preferences),LogCompare);
  preferences[i]=(char *) NULL;
  *number_preferences=(size_t) i;
  return(preferences);
}