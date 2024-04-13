MagickPrivate void XRefreshWindow(Display *display,const XWindowInfo *window,
  const XEvent *event)
{
  int
    x,
    y;

  unsigned int
    height,
    width;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window != (XWindowInfo *) NULL);
  if (window->ximage == (XImage *) NULL)
    return;
  if (event != (XEvent *) NULL)
    {
      /*
        Determine geometry from expose event.
      */
      x=event->xexpose.x;
      y=event->xexpose.y;
      width=(unsigned int) event->xexpose.width;
      height=(unsigned int) event->xexpose.height;
    }
  else
    {
      XEvent
        sans_event;

      /*
        Refresh entire window; discard outstanding expose events.
      */
      x=0;
      y=0;
      width=window->width;
      height=window->height;
      while (XCheckTypedWindowEvent(display,window->id,Expose,&sans_event)) ;
      if (window->matte_pixmap != (Pixmap) NULL)
        {
#if defined(MAGICKCORE_HAVE_SHAPE)
          if (window->shape != MagickFalse)
            XShapeCombineMask(display,window->id,ShapeBounding,0,0,
              window->matte_pixmap,ShapeSet);
#endif
        }
    }
  /*
    Check boundary conditions.
  */
  if ((window->ximage->width-(x+window->x)) < (int) width)
    width=(unsigned int) (window->ximage->width-(x+window->x));
  if ((window->ximage->height-(y+window->y)) < (int) height)
    height=(unsigned int) (window->ximage->height-(y+window->y));
  /*
    Refresh image.
  */
  if (window->matte_pixmap != (Pixmap) NULL)
    (void) XSetClipMask(display,window->annotate_context,window->matte_pixmap);
  if (window->pixmap != (Pixmap) NULL)
    {
      if (window->depth > 1)
        (void) XCopyArea(display,window->pixmap,window->id,
          window->annotate_context,x+window->x,y+window->y,width,height,x,y);
      else
        (void) XCopyPlane(display,window->pixmap,window->id,
          window->highlight_context,x+window->x,y+window->y,width,height,x,y,
          1L);
    }
  else
    {
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
      if (window->shared_memory)
        (void) XShmPutImage(display,window->id,window->annotate_context,
          window->ximage,x+window->x,y+window->y,x,y,width,height,MagickTrue);
#endif
      if (window->shared_memory == MagickFalse)
        (void) XPutImage(display,window->id,window->annotate_context,
          window->ximage,x+window->x,y+window->y,x,y,width,height);
    }
  if (window->matte_pixmap != (Pixmap) NULL)
    (void) XSetClipMask(display,window->annotate_context,None);
  (void) XFlush(display);
}