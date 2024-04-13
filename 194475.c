MagickExport void CloseMagickLog(void)
{
  ExceptionInfo
    *exception;

  LogInfo
    *log_info;

  if (IsEventLogging() == MagickFalse)
    return;
  exception=AcquireExceptionInfo();
  log_info=GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  LockSemaphoreInfo(log_semaphore);
  if (log_info->file != (FILE *) NULL)
    {
      (void) FormatLocaleFile(log_info->file,"</log>\n");
      (void) fclose(log_info->file);
      log_info->file=(FILE *) NULL;
    }
  UnlockSemaphoreInfo(log_semaphore);
}