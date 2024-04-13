MagickExport MagickBooleanType LogMagickEventList(const LogEventType type,
  const char *module,const char *function,const size_t line,const char *format,
  va_list operands)
{
  char
    event[MagickPathExtent],
    *text;

  const char
    *domain;

  ExceptionInfo
    *exception;

  int
    n;

  LogInfo
    *log_info;

  exception=AcquireExceptionInfo();
  log_info=(LogInfo *) GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  if (event_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&event_semaphore);
  LockSemaphoreInfo(event_semaphore);
  if ((log_info->event_mask & type) == 0)
    {
      UnlockSemaphoreInfo(event_semaphore);
      return(MagickTrue);
    }
  domain=CommandOptionToMnemonic(MagickLogEventOptions,type);
#if defined(MAGICKCORE_HAVE_VSNPRINTF)
  n=vsnprintf(event,MagickPathExtent,format,operands);
#else
  n=vsprintf(event,format,operands);
#endif
  if (n < 0)
    event[MagickPathExtent-1]='\0';
  text=TranslateEvent(module,function,line,domain,event);
  if (text == (char *) NULL)
    {
      (void) ContinueTimer((TimerInfo *) &log_info->timer);
      UnlockSemaphoreInfo(event_semaphore);
      return(MagickFalse);
    }
  if ((log_info->handler_mask & ConsoleHandler) != 0)
    {
      (void) FormatLocaleFile(stderr,"%s\n",text);
      (void) fflush(stderr);
    }
  if ((log_info->handler_mask & DebugHandler) != 0)
    {
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
      OutputDebugString(text);
      OutputDebugString("\n");
#endif
    }
  if ((log_info->handler_mask & EventHandler) != 0)
    {
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
      (void) NTReportEvent(text,MagickFalse);
#endif
    }
  if ((log_info->handler_mask & FileHandler) != 0)
    {
      struct stat
        file_info;

      file_info.st_size=0;
      if (log_info->file != (FILE *) NULL)
        (void) fstat(fileno(log_info->file),&file_info);
      if (file_info.st_size > (ssize_t) (1024*1024*log_info->limit))
        {
          (void) FormatLocaleFile(log_info->file,"</log>\n");
          (void) fclose(log_info->file);
          log_info->file=(FILE *) NULL;
        }
      if (log_info->file == (FILE *) NULL)
        {
          char
            *filename;

          filename=TranslateFilename(log_info);
          if (filename == (char *) NULL)
            {
              (void) ContinueTimer((TimerInfo *) &log_info->timer);
              UnlockSemaphoreInfo(event_semaphore);
              return(MagickFalse);
            }
          log_info->append=IsPathAccessible(filename);
          log_info->file=fopen_utf8(filename,"ab");
          filename=(char  *) RelinquishMagickMemory(filename);
          if (log_info->file == (FILE *) NULL)
            {
              UnlockSemaphoreInfo(event_semaphore);
              return(MagickFalse);
            }
          log_info->generation++;
          if (log_info->append == MagickFalse)
            (void) FormatLocaleFile(log_info->file,"<?xml version=\"1.0\" "
              "encoding=\"UTF-8\" standalone=\"yes\"?>\n");
          (void) FormatLocaleFile(log_info->file,"<log>\n");
        }
      (void) FormatLocaleFile(log_info->file,"  <event>%s</event>\n",text);
      (void) fflush(log_info->file);
    }
  if ((log_info->handler_mask & MethodHandler) != 0)
    {
      if (log_info->method != (MagickLogMethod) NULL)
        log_info->method(type,text);
    }
  if ((log_info->handler_mask & StdoutHandler) != 0)
    {
      (void) FormatLocaleFile(stdout,"%s\n",text);
      (void) fflush(stdout);
    }
  if ((log_info->handler_mask & StderrHandler) != 0)
    {
      (void) FormatLocaleFile(stderr,"%s\n",text);
      (void) fflush(stderr);
    }
  text=(char  *) RelinquishMagickMemory(text);
  (void) ContinueTimer((TimerInfo *) &log_info->timer);
  UnlockSemaphoreInfo(event_semaphore);
  return(MagickTrue);
}