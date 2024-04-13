MagickExport const char *GetImageProperty(const Image *image,
  const char *property,ExceptionInfo *exception)
{
  register const char
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  p=(const char *) NULL;
  if (image->properties != (void *) NULL)
    {
      if (property == (const char *) NULL)
        return((const char *) GetRootValueFromSplayTree((SplayTreeInfo *)
          image->properties));
      p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
        image->properties,property);
      if (p != (const char *) NULL)
        return(p);
    }
  if ((property == (const char *) NULL) ||
      (strchr(property,':') == (char *) NULL))
    return(p);
  switch (*property)
  {
    case '8':
    {
      if (LocaleNCompare("8bim:",property,5) == 0)
        {
          (void) Get8BIMProperty(image,property,exception);
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleNCompare("exif:",property,5) == 0)
        {
          (void) GetEXIFProperty(image,property,exception);
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if ((LocaleNCompare("icc:",property,4) == 0) ||
          (LocaleNCompare("icm:",property,4) == 0))
        {
          (void) GetICCProperty(image,property,exception);
          break;
        }
      if (LocaleNCompare("iptc:",property,5) == 0)
        {
          (void) GetIPTCProperty(image,property,exception);
          break;
        }
      break;
    }
    case 'X':
    case 'x':
    {
      if (LocaleNCompare("xmp:",property,4) == 0)
        {
          (void) GetXMPProperty(image,property);
          break;
        }
      break;
    }
    default:
      break;
  }
  if (image->properties != (void *) NULL)
    {
      p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
        image->properties,property);
      return(p);
    }
  return((const char *) NULL);
}