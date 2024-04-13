static MagickBooleanType LoadDelegateCache(LinkedListInfo *cache,
  const char *xml,const char *filename,const size_t depth,
  ExceptionInfo *exception)
{
  char
    keyword[MagickPathExtent],
    *token;

  const char
    *q;

  DelegateInfo
    *delegate_info;

  MagickStatusType
    status;

  size_t
    extent;

  /*
    Load the delegate map file.
  */
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Loading delegate configuration file \"%s\" ...",filename);
  if (xml == (const char *) NULL)
    return(MagickFalse);
  status=MagickTrue;
  delegate_info=(DelegateInfo *) NULL;
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
                      status&=LoadDelegateCache(cache,file_xml,path,
                        depth+1,exception);
                      file_xml=DestroyString(file_xml);
                    }
                }
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<delegate") == 0)
      {
        /*
          Delegate element.
        */
        delegate_info=(DelegateInfo *) AcquireCriticalMemory(
          sizeof(*delegate_info));
        (void) memset(delegate_info,0,sizeof(*delegate_info));
        delegate_info->path=ConstantString(filename);
        delegate_info->thread_support=MagickTrue;
        delegate_info->signature=MagickCoreSignature;
        continue;
      }
    if (delegate_info == (DelegateInfo *) NULL)
      continue;
    if ((LocaleCompare(keyword,"/>") == 0) ||
        (LocaleCompare(keyword,"</policy>") == 0))
      {
        status=AppendValueToLinkedList(cache,delegate_info);
        if (status == MagickFalse)
          (void) ThrowMagickException(exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'",
            delegate_info->commands);
        delegate_info=(DelegateInfo *) NULL;
        continue;
      }
    GetNextToken(q,(const char **) NULL,extent,token);
    if (*token != '=')
      continue;
    GetNextToken(q,&q,extent,token);
    GetNextToken(q,&q,extent,token);
    switch (*keyword)
    {
      case 'C':
      case 'c':
      {
        if (LocaleCompare((char *) keyword,"command") == 0)
          {
            char
              *commands;

            commands=AcquireString(token);
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
            if (strchr(commands,'@') != (char *) NULL)
              {
                char
                  path[MagickPathExtent];

                NTGhostscriptEXE(path,MagickPathExtent);
                (void) SubstituteString((char **) &commands,"@PSDelegate@",
                  path);
                (void) SubstituteString((char **) &commands,"\\","/");
              }
            (void) SubstituteString((char **) &commands,"&quot;","\"");
#else
            (void) SubstituteString((char **) &commands,"&quot;","'");
#endif
            (void) SubstituteString((char **) &commands,"&amp;","&");
            (void) SubstituteString((char **) &commands,"&gt;",">");
            (void) SubstituteString((char **) &commands,"&lt;","<");
            delegate_info->commands=commands;
            break;
          }
        break;
      }
      case 'D':
      case 'd':
      {
        if (LocaleCompare((char *) keyword,"decode") == 0)
          {
            delegate_info->decode=ConstantString(token);
            delegate_info->mode=1;
            break;
          }
        break;
      }
      case 'E':
      case 'e':
      {
        if (LocaleCompare((char *) keyword,"encode") == 0)
          {
            delegate_info->encode=ConstantString(token);
            delegate_info->mode=(-1);
            break;
          }
        break;
      }
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"mode") == 0)
          {
            delegate_info->mode=1;
            if (LocaleCompare(token,"bi") == 0)
              delegate_info->mode=0;
            else
              if (LocaleCompare(token,"encode") == 0)
                delegate_info->mode=(-1);
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"spawn") == 0)
          {
            delegate_info->spawn=IsStringTrue(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            delegate_info->stealth=IsStringTrue(token);
            break;
          }
        break;
      }
      case 'T':
      case 't':
      {
        if (LocaleCompare((char *) keyword,"thread-support") == 0)
          {
            delegate_info->thread_support=IsStringTrue(token);
            if (delegate_info->thread_support == MagickFalse)
              delegate_info->semaphore=AcquireSemaphoreInfo();
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  token=(char *) RelinquishMagickMemory(token);
  return(status != 0 ? MagickTrue : MagickFalse);
}