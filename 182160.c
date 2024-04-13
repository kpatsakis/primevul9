static MagickBooleanType XMakePixmap(Display *display,
  const XResourceInfo *resource_info,XWindowInfo *window)
{
  unsigned int
    height,
    width;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo  *) NULL);
  (void) resource_info;
  if (window->pixmap != (Pixmap) NULL)
    {
      /*
        Destroy previous X pixmap.
      */
      (void) XFreePixmap(display,window->pixmap);
      window->pixmap=(Pixmap) NULL;
    }
  if (window->use_pixmap == MagickFalse)
    return(MagickFalse);
  if (window->ximage == (XImage *) NULL)
    return(MagickFalse);
  /*
    Display busy cursor.
  */
  (void) XCheckDefineCursor(display,window->id,window->busy_cursor);
  (void) XFlush(display);
  /*
    Create pixmap.
  */
  width=(unsigned int) window->ximage->width;
  height=(unsigned int) window->ximage->height;
  window->pixmap=XCreatePixmap(display,window->id,width,height,window->depth);
  if (window->pixmap == (Pixmap) NULL)
    {
      /*
        Unable to allocate pixmap.
      */
      (void) XCheckDefineCursor(display,window->id,window->cursor);
      return(MagickFalse);
    }
  /*
    Copy X image to pixmap.
  */
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
  if (window->shared_memory)
    (void) XShmPutImage(display,window->pixmap,window->annotate_context,
      window->ximage,0,0,0,0,width,height,MagickTrue);
#endif
  if (window->shared_memory == MagickFalse)
    (void) XPutImage(display,window->pixmap,window->annotate_context,
      window->ximage,0,0,0,0,width,height);
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),"Pixmap:");
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  width, height: %ux%u",
        width,height);
    }
  /*
    Restore cursor.
  */
  (void) XCheckDefineCursor(display,window->id,window->cursor);
  return(MagickTrue);
}