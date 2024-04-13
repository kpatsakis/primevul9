MagickExport void XUserPreferences(XResourceInfo *resource_info)
{
#if defined(X11_PREFERENCES_PATH)
  char
    cache[MaxTextExtent],
    filename[MaxTextExtent],
    specifier[MaxTextExtent];

  const char
    *client_name,
    *value;

  XrmDatabase
    preferences_database;

  /*
    Save user preferences to the client configuration file.
  */
  assert(resource_info != (XResourceInfo *) NULL);
  client_name=GetClientName();
  preferences_database=XrmGetStringDatabase("");
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.backdrop",client_name);
  value=resource_info->backdrop ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.colormap",client_name);
  value=resource_info->colormap == SharedColormap ? "Shared" : "Private";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.confirmExit",
    client_name);
  value=resource_info->confirm_exit ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.confirmEdit",
    client_name);
  value=resource_info->confirm_edit ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.displayWarnings",
    client_name);
  value=resource_info->display_warnings ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.dither",client_name);
  value=resource_info->quantize_info->dither ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.gammaCorrect",
    client_name);
  value=resource_info->gamma_correct ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.undoCache",client_name);
  (void) FormatLocaleString(cache,MaxTextExtent,"%.20g",(double)
    resource_info->undo_cache);
  XrmPutStringResource(&preferences_database,specifier,cache);
  (void) FormatLocaleString(specifier,MaxTextExtent,"%s.usePixmap",client_name);
  value=resource_info->use_pixmap ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  (void) FormatLocaleString(filename,MaxTextExtent,"%s%src",
    X11_PREFERENCES_PATH,client_name);
  ExpandFilename(filename);
  XrmPutFileDatabase(preferences_database,filename);
#endif
}