static MagickBooleanType LoadLogCache(LinkedListInfo *cache,const char *xml,
  const char *filename,const size_t depth,ExceptionInfo *exception)
{
  char
    keyword[MagickPathExtent],
    *token;

  const char
    *q;

  LogInfo
    *log_info = (LogInfo *) NULL;

  MagickStatusType
    status;

  size_t
    extent;

  /*
    Load the log map file.
  */
  if (xml == (const char *) NULL)
    return(MagickFalse);
  status=MagickTrue;
  token=AcquireString(xml);
  extent=strlen(token)+MagickPathExtent;
  for (q=(const char *) xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetNextToken(q,&q,extent,token);
    if (*token == '\0')
      break;
    (void) CopyMagickString(keyword,token,MagickPathExtent);
    if (LocaleNCompare(keyword,"<!DOCTYPE",9) == 0)
      {
        /*
          Doctype element.
        */
        while ((LocaleNCompare(q,"]>",2) != 0) && (*q != '\0'))
          GetNextToken(q,&q,extent,token);
        continue;
      }
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetNextToken(q,&q,extent,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include element.
        */
        while (((*token != '/') && (*(token+1) != '>')) && (*q != '\0'))
        {
          (void) CopyMagickString(keyword,token,MagickPathExtent);
          GetNextToken(q,&q,extent,token);
          if (*token != '=')
            continue;
          GetNextToken(q,&q,extent,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > MagickMaxRecursionDepth)
                (void) ThrowMagickException(exception,GetMagickModule(),
                  ConfigureError,"IncludeElementNestedTooDeeply","`%s'",token);
              else
                {
                  char
                    path[MagickPathExtent],
                    *file_xml;

                  GetPathComponent(filename,HeadPath,path);
                  if (*path != '\0')
                    (void) ConcatenateMagickString(path,DirectorySeparator,
                      MagickPathExtent);
                  if (*token == *DirectorySeparator)
                    (void) CopyMagickString(path,token,MagickPathExtent);
                  else
                    (void) ConcatenateMagickString(path,token,MagickPathExtent);
                  file_xml=FileToXML(path,~0UL);
                  if (file_xml != (char *) NULL)
                    {
                      status&=LoadLogCache(cache,file_xml,path,depth+1,
                        exception);
                      file_xml=DestroyString(file_xml);
                    }
                }
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<logmap>") == 0)
      {
        /*
          Allocate memory for the log list.
        */
        log_info=(LogInfo *) AcquireCriticalMemory(sizeof(*log_info));
        (void) memset(log_info,0,sizeof(*log_info));
        log_info->path=ConstantString(filename);
        GetTimerInfo((TimerInfo *) &log_info->timer);
        log_info->signature=MagickCoreSignature;
        continue;
      }
    if (log_info == (LogInfo *) NULL)
      continue;
    if (LocaleCompare(keyword,"</logmap>") == 0)
      {
        status=AppendValueToLinkedList(cache,log_info);
        if (status == MagickFalse)
          (void) ThrowMagickException(exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'",filename);
        log_info=(LogInfo *) NULL;
        continue;
      }
    GetNextToken(q,(const char **) NULL,extent,token);
    if (*token != '=')
      continue;
    GetNextToken(q,&q,extent,token);
    GetNextToken(q,&q,extent,token);
    switch (*keyword)
    {
      case 'E':
      case 'e':
      {
        if (LocaleCompare((char *) keyword,"events") == 0)
          {
            log_info->event_mask=(LogEventType) (log_info->event_mask |
              ParseCommandOption(MagickLogEventOptions,MagickTrue,token));
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"filename") == 0)
          {
            if (log_info->filename != (char *) NULL)
              log_info->filename=(char *)
                RelinquishMagickMemory(log_info->filename);
            log_info->filename=ConstantString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            if (log_info->format != (char *) NULL)
              log_info->format=(char *)
                RelinquishMagickMemory(log_info->format);
            log_info->format=ConstantString(token);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"generations") == 0)
          {
            if (LocaleCompare(token,"unlimited") == 0)
              {
                log_info->generations=(~0UL);
                break;
              }
            log_info->generations=StringToUnsignedLong(token);
            break;
          }
        break;
      }
      case 'L':
      case 'l':
      {
        if (LocaleCompare((char *) keyword,"limit") == 0)
          {
            if (LocaleCompare(token,"unlimited") == 0)
              {
                log_info->limit=(~0UL);
                break;
              }
            log_info->limit=StringToUnsignedLong(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"output") == 0)
          {
            log_info->handler_mask=(LogHandlerType)
              (log_info->handler_mask | ParseLogHandlers(token));
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  token=DestroyString(token);
  if (cache == (LinkedListInfo *) NULL)
    return(MagickFalse);
  return(status != 0 ? MagickTrue : MagickFalse);
}