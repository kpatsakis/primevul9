MagickPrivate void XDestroyWindowColors(Display *display,Window window)
{
  Atom
    property,
    type;

  int
    format;

  Status
    status;

  unsigned char
    *data;

  unsigned long
    after,
    length;

  /*
    If there are previous resources on the root window, destroy them.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  property=XInternAtom(display,"_XSETROOT_ID",MagickFalse);
  if (property == (Atom) NULL)
    {
      ThrowXWindowException(XServerError,"UnableToCreateProperty",
        "_XSETROOT_ID");
      return;
    }
  status=XGetWindowProperty(display,window,property,0L,1L,MagickTrue,
    (Atom) AnyPropertyType,&type,&format,&length,&after,&data);
  if (status != Success)
    return;
  if ((type == XA_PIXMAP) && (format == 32) && (length == 1) && (after == 0))
    {
      (void) XKillClient(display,(XID) (*((Pixmap *) data)));
      (void) XDeleteProperty(display,window,property);
    }
  if (type != None)
    (void) XFree((void *) data);
}