MagickExport int XError(Display *display,XErrorEvent *error)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(error != (XErrorEvent *) NULL);
  xerror_alert=MagickTrue;
  switch (error->request_code)
  {
    case X_GetGeometry:
    {
      if ((int) error->error_code == BadDrawable)
        return(MagickFalse);
      break;
    }
    case X_GetWindowAttributes:
    case X_QueryTree:
    {
      if ((int) error->error_code == BadWindow)
        return(MagickFalse);
      break;
    }
    case X_QueryColors:
    {
      if ((int) error->error_code == BadValue)
        return(MagickFalse);
      break;
    }
  }
  return(MagickTrue);
}