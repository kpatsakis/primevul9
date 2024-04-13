MagickExport MagickBooleanType AnimateImages(const ImageInfo *image_info,
  Image *images)
{
  char
    *argv[1];

  Display
    *display;

  MagickStatusType
    status;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource_info;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    {
      (void) ThrowMagickException(&images->exception,GetMagickModule(),
        XServerError,"UnableToOpenXServer","`%s'",XDisplayName(
        image_info->server_name));
      return(MagickFalse);
    }
  if (images->exception.severity != UndefinedException)
    CatchException(&images->exception);
  (void) XSetErrorHandler(XError);
  resource_database=XGetResourceDatabase(display,GetClientName());
  (void) memset(&resource_info,0,sizeof(XResourceInfo));
  XGetResourceInfo(image_info,resource_database,GetClientName(),&resource_info);
  if (image_info->page != (char *) NULL)
    resource_info.image_geometry=AcquireString(image_info->page);
  resource_info.immutable=MagickTrue;
  argv[0]=AcquireString(GetClientName());
  (void) XAnimateImages(display,&resource_info,argv,1,images);
  (void) SetErrorHandler((ErrorHandler) NULL);
  (void) SetWarningHandler((WarningHandler) NULL);
  argv[0]=DestroyString(argv[0]);
  (void) XCloseDisplay(display);
  XDestroyResourceInfo(&resource_info);
  status=images->exception.severity == UndefinedException ?
    MagickTrue : MagickFalse;
  return(status != 0 ? MagickTrue : MagickFalse);
}