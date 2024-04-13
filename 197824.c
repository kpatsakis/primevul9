compileWarning (FileInfo * nested, char *format, ...)
{
#ifndef __SYMBIAN32__
  char buffer[MAXSTRING];
  va_list arguments;
  va_start (arguments, format);
#ifdef _WIN32
  _vsnprintf (buffer, sizeof (buffer), format, arguments);
#else
  vsnprintf (buffer, sizeof (buffer), format, arguments);
#endif
  va_end (arguments);
  if (nested)
    lou_logPrint ("%s:%d: warning: %s", nested->fileName,
		  nested->lineNumber, buffer);
  else
    lou_logPrint ("warning: %s", buffer);
  warningCount++;
#endif
}