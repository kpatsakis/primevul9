static LinkedListInfo *AcquireDelegateCache(const char *filename,
  ExceptionInfo *exception)
{
  LinkedListInfo
    *cache;

  MagickStatusType
    status;

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
      status&=LoadDelegateCache(cache,(const char *)
        GetStringInfoDatum(option),GetStringInfoPath(option),0,exception);
      option=(const StringInfo *) GetNextValueInLinkedList(options);
    }
    options=DestroyConfigureOptions(options);
  }
#endif
  if (IsLinkedListEmpty(cache) != MagickFalse)
    status&=LoadDelegateCache(cache,DelegateMap,"built-in",0,exception);
  return(cache);
}