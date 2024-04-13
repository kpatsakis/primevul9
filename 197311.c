static const char *GetLocaleMonitorMessage(const char *text)
{
  char
    message[MagickPathExtent],
    tag[MagickPathExtent];

  const char
    *locale_message;

  register char
    *p;

  (void) CopyMagickString(tag,text,MagickPathExtent);
  p=strrchr(tag,'/');
  if (p != (char *) NULL)
    *p='\0';
  (void) FormatLocaleString(message,MagickPathExtent,"Monitor/%s",tag);
  locale_message=GetLocaleMessage(message);
  if (locale_message == message)
    return(text);
  return(locale_message);
}