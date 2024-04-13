MagickExport char *XGetResourceInstance(XrmDatabase database,
  const char *client_name,const char *keyword,const char *resource_default)
{
  char
    *resource_type,
    resource_name[MaxTextExtent];

  Status
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return((char *) resource_default);
  *resource_name='\0';
  if (keyword != (char *) NULL)
    (void) FormatLocaleString(resource_name,MaxTextExtent,"%s.%s",client_name,
      keyword);
  status=XrmGetResource(database,resource_name,"ImageMagick",&resource_type,
    &resource_value);
  if (status == False)
    return((char *) resource_default);
  return(resource_value.addr);
}