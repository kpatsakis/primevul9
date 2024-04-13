MagickExport XrmDatabase XGetResourceDatabase(Display *display,
  const char *client_name)
{
  char
    filename[MagickPathExtent];

  int
    c;

  register const char
    *p;

  XrmDatabase
    resource_database,
    server_database;

  if (display == (Display *) NULL)
    return((XrmDatabase) NULL);
  assert(client_name != (char *) NULL);
  /*
    Initialize resource database.
  */
  XrmInitialize();
  (void) XGetDefault(display,(char *) client_name,"dummy");
  resource_database=XrmGetDatabase(display);
  /*
    Combine application database.
  */
  p=client_name+(strlen(client_name)-1);
  while ((p > client_name) && (*p != '/'))
    p--;
  if (*p == '/')
    client_name=p+1;
  c=(int) (*client_name);
  if ((c >= XK_a) && (c <= XK_z))
    c-=(XK_a-XK_A);
  else
    if ((c >= XK_agrave) && (c <= XK_odiaeresis))
      c-=(XK_agrave-XK_Agrave);
    else
      if ((c >= XK_oslash) && (c <= XK_thorn))
        c-=(XK_oslash-XK_Ooblique);
#if defined(X11_APPLICATION_PATH)
  (void) FormatLocaleString(filename,MagickPathExtent,"%s%c%s",
    X11_APPLICATION_PATH,c,client_name+1);
  (void) XrmCombineFileDatabase(filename,&resource_database,MagickFalse);
#endif
  if (XResourceManagerString(display) != (char *) NULL)
    {
      /*
        Combine server database.
      */
      server_database=XrmGetStringDatabase(XResourceManagerString(display));
      XrmCombineDatabase(server_database,&resource_database,MagickFalse);
    }
  /*
    Merge user preferences database.
  */
#if defined(X11_PREFERENCES_PATH)
  (void) FormatLocaleString(filename,MagickPathExtent,"%s%src",
    X11_PREFERENCES_PATH,client_name);
  ExpandFilename(filename);
  (void) XrmCombineFileDatabase(filename,&resource_database,MagickFalse);
#endif
  return(resource_database);
}