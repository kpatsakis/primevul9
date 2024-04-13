MagickPrivate int XCheckDefineCursor(Display *display,Window window,
  Cursor cursor)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  if (window == XRootWindow(display,XDefaultScreen(display)))
    return(0);
  return(XDefineCursor(display,window,cursor));
}