static MagickBooleanType IsLogCacheInstantiated(ExceptionInfo *exception)
{
  if (log_cache == (LinkedListInfo *) NULL)
    {
      if (log_semaphore == (SemaphoreInfo *) NULL)
        ActivateSemaphoreInfo(&log_semaphore);
      LockSemaphoreInfo(log_semaphore);
      if (log_cache == (LinkedListInfo *) NULL)
        {
          log_cache=AcquireLogCache(LogFilename,exception);
          CheckEventLogging();
        }
      UnlockSemaphoreInfo(log_semaphore);
    }
  return(log_cache != (LinkedListInfo *) NULL ? MagickTrue : MagickFalse);
}