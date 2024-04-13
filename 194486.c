MagickExport void SetLogMethod(MagickLogMethod method)
{
  ExceptionInfo
    *exception;

  LogInfo
    *log_info;

  exception=AcquireExceptionInfo();
  log_info=(LogInfo *) GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  LockSemaphoreInfo(log_semaphore);
  log_info=(LogInfo *) GetValueFromLinkedList(log_cache,0);
  log_info->handler_mask=(LogHandlerType) (log_info->handler_mask |
    MethodHandler);
  log_info->method=method;
  UnlockSemaphoreInfo(log_semaphore);
}