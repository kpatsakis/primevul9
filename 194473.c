static char *TranslateEvent(const char *module,const char *function,
  const size_t line,const char *domain,const char *event)
{
  char
    *text;

  double
    elapsed_time,
    user_time;

  ExceptionInfo
    *exception;

  LogInfo
    *log_info;

  register char
    *q;

  register const char
    *p;

  size_t
    extent;

  time_t
    seconds;

  exception=AcquireExceptionInfo();
  log_info=(LogInfo *) GetLogInfo("*",exception);
  exception=DestroyExceptionInfo(exception);
  seconds=time((time_t *) NULL);
  elapsed_time=GetElapsedTime(&log_info->timer);
  user_time=GetUserTime(&log_info->timer);
  text=AcquireString(event);
  if (log_info->format == (char *) NULL)
    return(text);
  extent=strlen(event)+MagickPathExtent;
  if (LocaleCompare(log_info->format,"xml") == 0)
    {
      char
        timestamp[MagickPathExtent];

      /*
        Translate event in "XML" format.
      */
      (void) FormatMagickTime(seconds,extent,timestamp);
      (void) FormatLocaleString(text,extent,
        "<entry>\n"
        "  <timestamp>%s</timestamp>\n"
        "  <elapsed-time>%lu:%02lu.%03lu</elapsed-time>\n"
        "  <user-time>%0.3f</user-time>\n"
        "  <process-id>%.20g</process-id>\n"
        "  <thread-id>%.20g</thread-id>\n"
        "  <module>%s</module>\n"
        "  <function>%s</function>\n"
        "  <line>%.20g</line>\n"
        "  <domain>%s</domain>\n"
        "  <event>%s</event>\n"
        "</entry>",timestamp,(unsigned long) (elapsed_time/60.0),
        (unsigned long) floor(fmod(elapsed_time,60.0)),(unsigned long)
        (1000.0*(elapsed_time-floor(elapsed_time))+0.5),user_time,
        (double) getpid(),(double) GetMagickThreadSignature(),module,function,
        (double) line,domain,event);
      return(text);
    }
  /*
    Translate event in "human readable" format.
  */
  q=text;
  for (p=log_info->format; *p != '\0'; p++)
  {
    *q='\0';
    if ((size_t) (q-text+MagickPathExtent) >= extent)
      {
        extent+=MagickPathExtent;
        text=(char *) ResizeQuantumMemory(text,extent+MagickPathExtent,
          sizeof(*text));
        if (text == (char *) NULL)
          return((char *) NULL);
        q=text+strlen(text);
      }
    /*
      The format of the log is defined by embedding special format characters:

        %c   client name
        %d   domain
        %e   event
        %f   function
        %g   generation
        %l   line
        %m   module
        %n   log name
        %p   process id
        %r   real CPU time
        %t   wall clock time
        %u   user CPU time
        %v   version
        %%   percent sign
        \n   newline
        \r   carriage return
    */
    if ((*p == '\\') && (*(p+1) == 'r'))
      {
        *q++='\r';
        p++;
        continue;
      }
    if ((*p == '\\') && (*(p+1) == 'n'))
      {
        *q++='\n';
        p++;
        continue;
      }
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
      case 'd':
      {
        q+=CopyMagickString(q,domain,extent);
        break;
      }
      case 'e':
      {
        q+=CopyMagickString(q,event,extent);
        break;
      }
      case 'f':
      {
        q+=CopyMagickString(q,function,extent);
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
      case 'l':
      {
        q+=FormatLocaleString(q,extent,"%.20g",(double) line);
        break;
      }
      case 'm':
      {
        register const char
          *r;

        for (r=module+strlen(module)-1; r > module; r--)
          if (*r == *DirectorySeparator)
            {
              r++;
              break;
            }
        q+=CopyMagickString(q,r,extent);
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
      case 'r':
      {
        q+=FormatLocaleString(q,extent,"%lu:%02lu.%03lu",(unsigned long)
          (elapsed_time/60.0),(unsigned long) floor(fmod(elapsed_time,60.0)),
          (unsigned long) (1000.0*(elapsed_time-floor(elapsed_time))+0.5));
        break;
      }
      case 't':
      {
        q+=FormatMagickTime(seconds,extent,q);
        break;
      }
      case 'u':
      {
        q+=FormatLocaleString(q,extent,"%0.3fu",user_time);
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
  return(text);
}