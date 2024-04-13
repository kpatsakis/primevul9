static char *TranslateFilename(const LogInfo *log_info)
{
  char
    *filename;

  register char
    *q;

  register const char
    *p;

  size_t
    extent;

  /*
    Translate event in "human readable" format.
  */
  assert(log_info != (LogInfo *) NULL);
  assert(log_info->filename != (char *) NULL);
  filename=AcquireString((char *) NULL);
  extent=MagickPathExtent;
  q=filename;
  for (p=log_info->filename; *p != '\0'; p++)
  {
    *q='\0';
    if ((size_t) (q-filename+MagickPathExtent) >= extent)
      {
        extent+=MagickPathExtent;
        filename=(char *) ResizeQuantumMemory(filename,extent+MagickPathExtent,
          sizeof(*filename));
        if (filename == (char *) NULL)
          return((char *) NULL);
        q=filename+strlen(filename);
      }
    /*
      The format of the filename is defined by embedding special format
      characters:

        %c   client name
        %n   log name
        %p   process id
        %v   version
        %%   percent sign
    */
    if (*p != '%')
      {
        *q++=(*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'c':
      {
        q+=CopyMagickString(q,GetClientName(),extent);
        break;
      }
      case 'g':
      {
        if (log_info->generations == 0)
          {
            (void) CopyMagickString(q,"0",extent);
            q++;
            break;
          }
        q+=FormatLocaleString(q,extent,"%.20g",(double) (log_info->generation %
          log_info->generations));
        break;
      }
      case 'n':
      {
        q+=CopyMagickString(q,GetLogName(),extent);
        break;
      }
      case 'p':
      {
        q+=FormatLocaleString(q,extent,"%.20g",(double) getpid());
        break;
      }
      case 'v':
      {
        q+=CopyMagickString(q,MagickLibVersionText,extent);
        break;
      }
      case '%':
      {
        *q++=(*p);
        break;
      }
      default:
      {
        *q++='%';
        *q++=(*p);
        break;
      }
    }
  }
  *q='\0';
  return(filename);
}