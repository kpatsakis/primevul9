lou_logPrint (char *format, ...)
{
#ifndef __SYMBIAN32__
  va_list argp;
  if (format == NULL)
    return;
  if (logFile == NULL && initialLogFileName[0] != 0)
    logFile = fopen (initialLogFileName, "wb");
  if (logFile == NULL)
    logFile = stderr;
  va_start (argp, format);
  vfprintf (logFile, format, argp);
  fprintf (logFile, "\n");
  va_end (argp);
#endif
}