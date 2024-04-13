MagickExport const char *SetLogName(const char *name)
{
  if ((name != (char *) NULL) && (*name != '\0'))
    (void) CopyMagickString(log_name,name,MagickPathExtent);
  return(log_name);
}