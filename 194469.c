static LogHandlerType ParseLogHandlers(const char *handlers)
{
  LogHandlerType
    handler_mask;

  register const char
    *p;

  register ssize_t
    i;

  size_t
    length;

  handler_mask=NoHandler;
  for (p=handlers; p != (char *) NULL; p=strchr(p,','))
  {
    while ((*p != '\0') && ((isspace((int) ((unsigned char) *p)) != 0) ||
           (*p == ',')))
      p++;
    for (i=0; LogHandlers[i].name != (char *) NULL; i++)
    {
      length=strlen(LogHandlers[i].name);
      if (LocaleNCompare(p,LogHandlers[i].name,length) == 0)
        {
          handler_mask=(LogHandlerType) (handler_mask | LogHandlers[i].handler);
          break;
        }
    }
    if (LogHandlers[i].name == (char *) NULL)
      return(UndefinedHandler);
  }
  return(handler_mask);
}