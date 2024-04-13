MagickExport MagickBooleanType ListThresholdMaps(FILE *file,
  ExceptionInfo *exception)
{
  const StringInfo
    *option;

  LinkedListInfo
    *options;

  MagickStatusType
    status;

  status=MagickTrue;
  if (file == (FILE *) NULL)
    file=stdout;
  options=GetConfigureOptions(ThresholdsFilename,exception);
  (void) FormatLocaleFile(file,
    "\n   Threshold Maps for Ordered Dither Operations\n");
  option=(const StringInfo *) GetNextValueInLinkedList(options);
  while (option != (const StringInfo *) NULL)
  {
    (void) FormatLocaleFile(file,"\nPath: %s\n\n",GetStringInfoPath(option));
    status&=ListThresholdMapFile(file,(const char *) GetStringInfoDatum(option),
      GetStringInfoPath(option),exception);
    option=(const StringInfo *) GetNextValueInLinkedList(options);
  }
  options=DestroyConfigureOptions(options);
  return(status != 0 ? MagickTrue : MagickFalse);
}