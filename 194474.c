static LinkedListInfo *AcquireLogCache(const char *filename,
  ExceptionInfo *exception)
{
  LinkedListInfo
    *cache;

  MagickStatusType
    status;

  register ssize_t
    i;

  /*
    Load external log map.
  */
  cache=NewLinkedList(0);
  status=MagickTrue;
#if !defined(MAGICKCORE_ZERO_CONFIGURATION_SUPPORT)
  {
    const StringInfo
      *option;

    LinkedListInfo
      *options;

    options=GetConfigureOptions(filename,exception);
    option=(const StringInfo *) GetNextValueInLinkedList(options);
    while (option != (const StringInfo *) NULL)
    {
      status&=LoadLogCache(cache,(const char *) GetStringInfoDatum(option),
        GetStringInfoPath(option),0,exception);
      option=(const StringInfo *) GetNextValueInLinkedList(options);
    }
    options=DestroyConfigureOptions(options);
  }
#endif
  /*
    Load built-in log map.
  */
  for (i=0; i < (ssize_t) (sizeof(LogMap)/sizeof(*LogMap)); i++)
  {
    LogInfo
      *log_info;

    register const LogMapInfo
      *p;

    p=LogMap+i;
    log_info=(LogInfo *) AcquireMagickMemory(sizeof(*log_info));
    if (log_info == (LogInfo *) NULL)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'",p->filename);
        continue;
      }
    (void) memset(log_info,0,sizeof(*log_info));
    log_info->path=ConstantString("[built-in]");
    GetTimerInfo((TimerInfo *) &log_info->timer);
    log_info->event_mask=p->event_mask;
    log_info->handler_mask=p->handler_mask;
    log_info->filename=ConstantString(p->filename);
    log_info->format=ConstantString(p->format);
    log_info->signature=MagickCoreSignature;
    status&=AppendValueToLinkedList(cache,log_info);
    if (status == MagickFalse)
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",log_info->name);
  }
  return(cache);
}