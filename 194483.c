MagickExport MagickBooleanType ListLogInfo(FILE *file,ExceptionInfo *exception)
{
#define MegabytesToBytes(value) ((MagickSizeType) (value)*1024*1024)

  const char
    *path;

  const LogInfo
    **log_info;

  register ssize_t
    i;

  size_t
    number_aliases;

  ssize_t
    j;

  if (file == (const FILE *) NULL)
    file=stdout;
  log_info=GetLogInfoList("*",&number_aliases,exception);
  if (log_info == (const LogInfo **) NULL)
    return(MagickFalse);
  j=0;
  path=(const char *) NULL;
  for (i=0; i < (ssize_t) number_aliases; i++)
  {
    if (log_info[i]->stealth != MagickFalse)
      continue;
    if ((path == (const char *) NULL) ||
        (LocaleCompare(path,log_info[i]->path) != 0))
      {
        size_t
          length;

        if (log_info[i]->path != (char *) NULL)
          (void) FormatLocaleFile(file,"\nPath: %s\n\n",log_info[i]->path);
        length=0;
        for (j=0; j < (ssize_t) (8*sizeof(LogHandlerType)); j++)
        {
          size_t
            mask;

          if (LogHandlers[j].name == (const char *) NULL)
            break;
          mask=1;
          mask<<=j;
          if ((log_info[i]->handler_mask & mask) != 0)
            {
              (void) FormatLocaleFile(file,"%s ",LogHandlers[j].name);
              length+=strlen(LogHandlers[j].name);
            }
        }
        for (j=(ssize_t) length; j <= 12; j++)
          (void) FormatLocaleFile(file," ");
        (void) FormatLocaleFile(file," Generations     Limit  Format\n");
        (void) FormatLocaleFile(file,"-----------------------------------------"
          "--------------------------------------\n");
      }
    path=log_info[i]->path;
    if (log_info[i]->filename != (char *) NULL)
      {
        (void) FormatLocaleFile(file,"%s",log_info[i]->filename);
        for (j=(ssize_t) strlen(log_info[i]->filename); j <= 16; j++)
          (void) FormatLocaleFile(file," ");
      }
    (void) FormatLocaleFile(file,"%9g  ",(double) log_info[i]->generations);
    (void) FormatLocaleFile(file,"%8g   ",(double) log_info[i]->limit);
    if (log_info[i]->format != (char *) NULL)
      (void) FormatLocaleFile(file,"%s",log_info[i]->format);
    (void) FormatLocaleFile(file,"\n");
  }
  (void) fflush(file);
  log_info=(const LogInfo **) RelinquishMagickMemory((void *) log_info);
  return(MagickTrue);
}