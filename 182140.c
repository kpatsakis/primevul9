MagickExport void XCheckRefreshWindows(Display *display,XWindows *windows)
{
  Window
    id;

  XEvent
    event;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  XDelay(display,SuspendTime);
  id=windows->command.id;
  while (XCheckTypedWindowEvent(display,id,Expose,&event) != MagickFalse)
    (void) XCommandWidget(display,windows,(char const **) NULL,&event);
  id=windows->image.id;
  while (XCheckTypedWindowEvent(display,id,Expose,&event) != MagickFalse)
    XRefreshWindow(display,&windows->image,&event);
  XDelay(display,SuspendTime << 1);
  id=windows->command.id;
  while (XCheckTypedWindowEvent(display,id,Expose,&event) != MagickFalse)
    (void) XCommandWidget(display,windows,(char const **) NULL,&event);
  id=windows->image.id;
  while (XCheckTypedWindowEvent(display,id,Expose,&event) != MagickFalse)
    XRefreshWindow(display,&windows->image,&event);
}