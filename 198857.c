MagickExport ThresholdMap *GetThresholdMap(const char *map_id,
  ExceptionInfo *exception)
{
  ThresholdMap
    *map;

  map=GetThresholdMapFile(MinimalThresholdMap,"built-in",map_id,exception);
  if (map != (ThresholdMap *) NULL)
    return(map);
#if !defined(MAGICKCORE_ZERO_CONFIGURATION_SUPPORT)
  {
    const StringInfo
      *option;

    LinkedListInfo
      *options;

    options=GetConfigureOptions(ThresholdsFilename,exception);
    option=(const StringInfo *) GetNextValueInLinkedList(options);
    while (option != (const StringInfo *) NULL)
    {
      map=GetThresholdMapFile((const char *) GetStringInfoDatum(option),
        GetStringInfoPath(option),map_id,exception);
      if (map != (ThresholdMap *) NULL)
        break;
      option=(const StringInfo *) GetNextValueInLinkedList(options);
    }
    options=DestroyConfigureOptions(options);
  }
#endif
  return(map);
}