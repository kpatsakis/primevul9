static MagickBooleanType IsDelegateCacheInstantiated(ExceptionInfo *exception)
{
  if (delegate_cache == (LinkedListInfo *) NULL)
    {
      if (delegate_semaphore == (SemaphoreInfo *) NULL)
        ActivateSemaphoreInfo(&delegate_semaphore);
      LockSemaphoreInfo(delegate_semaphore);
      if (delegate_cache == (LinkedListInfo *) NULL)
        delegate_cache=AcquireDelegateCache(DelegateFilename,exception);
      UnlockSemaphoreInfo(delegate_semaphore);
    }
  return(delegate_cache != (LinkedListInfo *) NULL ? MagickTrue : MagickFalse);
}