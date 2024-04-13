MagickExport LogEventType SetLogEventMask(const char *events)
{
  ExceptionInfo
    *exception;

  LogInfo
    *log_info;

  ssize_t
    option;

  exception=AcquireExceptionInfo();
  log_info=(LogInfo *) GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  option=ParseCommandOption(MagickLogEventOptions,MagickTrue,events);
  LockSemaphoreInfo(log_semaphore);
  log_info=(LogInfo *) GetValueFromLinkedList(log_cache,0);
  log_info->event_mask=(LogEventType) option;
  if (option == -1)
    log_info->event_mask=UndefinedEvents;
  CheckEventLogging();
  UnlockSemaphoreInfo(log_semaphore);
  return(log_info->event_mask);
}