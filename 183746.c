MagickExport MagickBooleanType DefineImageRegistry(const RegistryType type,
  const char *option,ExceptionInfo *exception)
{
  char
    key[MagickPathExtent],
    value[MagickPathExtent];

  register char
    *p;

  assert(option != (const char *) NULL);
  (void) CopyMagickString(key,option,MagickPathExtent);
  for (p=key; *p != '\0'; p++)
    if (*p == '=')
      break;
  *value='\0';
  if (*p == '=')
    (void) CopyMagickString(value,p+1,MagickPathExtent);
  *p='\0';
  return(SetImageRegistry(type,key,value,exception));
}