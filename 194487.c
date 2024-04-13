MagickExport void SetLogFormat(const char *format)
{
  LogInfo
    *log_info;

  ExceptionInfo
    *exception;

  exception=AcquireExceptionInfo();
  log_info=(LogInfo *) GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  LockSemaphoreInfo(log_semaphore);
  if (log_info->format != (char *) NULL)
    log_info->format=DestroyString(log_info->format);
  log_info->format=ConstantString(format);
  UnlockSemaphoreInfo(log_semaphore);
}