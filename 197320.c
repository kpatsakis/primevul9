MagickExport char *XGetResourceClass(XrmDatabase database,
  const char *client_name,const char *keyword,char *resource_default)
{
  char
    resource_class[MagickPathExtent],
    resource_name[MagickPathExtent];

  static char
    *resource_type;

  Status
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return(resource_default);
  *resource_name='\0';
  *resource_class='\0';
  if (keyword != (char *) NULL)
    {
      int
        c,
        k;

      /*
        Initialize resource keyword and class.
      */
      (void) FormatLocaleString(resource_name,MagickPathExtent,"%s.%s",
        client_name,keyword);
      c=(int) (*client_name);
      if ((c >= XK_a) && (c <= XK_z))
        c-=(XK_a-XK_A);
      else
        if ((c >= XK_agrave) && (c <= XK_odiaeresis))
          c-=(XK_agrave-XK_Agrave);
        else
          if ((c >= XK_oslash) && (c <= XK_thorn))
            c-=(XK_oslash-XK_Ooblique);
      k=(int) (*keyword);
      if ((k >= XK_a) && (k <= XK_z))
        k-=(XK_a-XK_A);
      else
        if ((k >= XK_agrave) && (k <= XK_odiaeresis))
          k-=(XK_agrave-XK_Agrave);
        else
          if ((k >= XK_oslash) && (k <= XK_thorn))
            k-=(XK_oslash-XK_Ooblique);
      (void) FormatLocaleString(resource_class,MagickPathExtent,"%c%s.%c%s",c,
        client_name+1,k,keyword+1);
    }
  status=XrmGetResource(database,resource_name,resource_class,&resource_type,
    &resource_value);
  if (status == False)
    return(resource_default);
  return(resource_value.addr);
}