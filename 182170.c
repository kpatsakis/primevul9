static const char *GetLocaleMonitorMessage(const char *text)
{
  char
    message[MaxTextExtent],
    tag[MaxTextExtent];

  const char
    *locale_message;

  register char
    *p;

  (void) CopyMagickString(tag,text,MaxTextExtent);
  p=strrchr(tag,'/');
  if (p != (char *) NULL)
    *p='\0';
  (void) FormatLocaleString(message,MaxTextExtent,"Monitor/%s",tag);
  locale_message=GetLocaleMessage(message);
  if (locale_message == message)
    return(text);
  return(locale_message);
}