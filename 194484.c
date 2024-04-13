MagickPrivate MagickBooleanType LogComponentGenesis(void)
{
  ExceptionInfo
    *exception;

  if (log_semaphore == (SemaphoreInfo *) NULL)
    log_semaphore=AcquireSemaphoreInfo();
  exception=AcquireExceptionInfo();
  (void) GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  event_semaphore=AcquireSemaphoreInfo();
  return(MagickTrue);
}