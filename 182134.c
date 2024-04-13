MagickExport void XRetainWindowColors(Display *display,const Window window)
{
  Atom
    property;

  Pixmap
    pixmap;

  /*
    Put property on the window.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  property=XInternAtom(display,"_XSETROOT_ID",MagickFalse);
  if (property == (Atom) NULL)
    ThrowXWindowFatalException(XServerError,"UnableToCreateProperty",
      "_XSETROOT_ID");
  pixmap=XCreatePixmap(display,window,1,1,1);
  if (pixmap == (Pixmap) NULL)
    ThrowXWindowFatalException(XServerError,"UnableToCreateBitmap","");
  (void) XChangeProperty(display,window,property,XA_PIXMAP,32,PropModeReplace,
    (unsigned char *) &pixmap,1);
  (void) XSetCloseDownMode(display,RetainPermanent);
}