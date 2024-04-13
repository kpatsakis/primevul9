MagickExport MagickBooleanType DefineImageProperty(Image *image,
  const char *property,ExceptionInfo *exception)
{
  char
    key[MagickPathExtent],
    value[MagickPathExtent];

  register char
    *p;

  assert(image != (Image *) NULL);
  assert(property != (const char *) NULL);
  (void) CopyMagickString(key,property,MagickPathExtent-1);
  for (p=key; *p != '\0'; p++)
    if (*p == '=')
      break;
  *value='\0';
  if (*p == '=')
    (void) CopyMagickString(value,p+1,MagickPathExtent);
  *p='\0';
  return(SetImageProperty(image,key,value,exception));
}