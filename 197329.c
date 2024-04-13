MagickPrivate void XGetWindowInfo(Display *display,XVisualInfo *visual_info,
  XStandardColormap *map_info,XPixelInfo *pixel,XFontStruct *font_info,
  XResourceInfo *resource_info,XWindowInfo *window)
{
  /*
    Initialize window info.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  if (window->id != (Window) NULL)
    {
      if (window->cursor != (Cursor) NULL)
        (void) XFreeCursor(display,window->cursor);
      if (window->busy_cursor != (Cursor) NULL)
        (void) XFreeCursor(display,window->busy_cursor);
      if (window->highlight_stipple != (Pixmap) NULL)
        (void) XFreePixmap(display,window->highlight_stipple);
      if (window->shadow_stipple != (Pixmap) NULL)
        (void) XFreePixmap(display,window->shadow_stipple);
      if (window->name == (char *) NULL)
        window->name=AcquireString("");
      if (window->icon_name == (char *) NULL)
        window->icon_name=AcquireString("");
    }
  else
    {
      /*
        Initialize these attributes just once.
      */
      window->id=(Window) NULL;
      if (window->name == (char *) NULL)
        window->name=AcquireString("");
      if (window->icon_name == (char *) NULL)
        window->icon_name=AcquireString("");
      window->x=XDisplayWidth(display,visual_info->screen) >> 1;
      window->y=XDisplayWidth(display,visual_info->screen) >> 1;
      window->ximage=(XImage *) NULL;
      window->matte_image=(XImage *) NULL;
      window->pixmap=(Pixmap) NULL;
      window->matte_pixmap=(Pixmap) NULL;
      window->mapped=MagickFalse;
      window->stasis=MagickFalse;
      window->shared_memory=MagickTrue;
      window->segment_info=(void *) NULL;
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
      {
        XShmSegmentInfo
          *segment_info;

        if (window->segment_info == (void *) NULL)
          window->segment_info=AcquireCriticalMemory(2*sizeof(*segment_info));
        segment_info=(XShmSegmentInfo *) window->segment_info;
        segment_info[0].shmid=(-1);
        segment_info[0].shmaddr=(char *) NULL;
        segment_info[1].shmid=(-1);
        segment_info[1].shmaddr=(char *) NULL;
      }
#endif
    }
  /*
    Initialize these attributes every time function is called.
  */
  window->screen=visual_info->screen;
  window->root=XRootWindow(display,visual_info->screen);
  window->visual=visual_info->visual;
  window->storage_class=(unsigned int) visual_info->klass;
  window->depth=(unsigned int) visual_info->depth;
  window->visual_info=visual_info;
  window->map_info=map_info;
  window->pixel_info=pixel;
  window->font_info=font_info;
  window->cursor=XCreateFontCursor(display,XC_left_ptr);
  window->busy_cursor=XCreateFontCursor(display,XC_watch);
  window->geometry=(char *) NULL;
  window->icon_geometry=(char *) NULL;
  if (resource_info->icon_geometry != (char *) NULL)
    (void) CloneString(&window->icon_geometry,resource_info->icon_geometry);
  window->crop_geometry=(char *) NULL;
  window->flags=(size_t) PSize;
  window->width=1;
  window->height=1;
  window->min_width=1;
  window->min_height=1;
  window->width_inc=1;
  window->height_inc=1;
  window->border_width=resource_info->border_width;
  window->annotate_context=pixel->annotate_context;
  window->highlight_context=pixel->highlight_context;
  window->widget_context=pixel->widget_context;
  window->shadow_stipple=(Pixmap) NULL;
  window->highlight_stipple=(Pixmap) NULL;
  window->use_pixmap=MagickTrue;
  window->immutable=MagickFalse;
  window->shape=MagickFalse;
  window->data=0;
  window->mask=(int) (CWBackingStore | CWBackPixel | CWBackPixmap |
    CWBitGravity | CWBorderPixel | CWColormap | CWCursor | CWDontPropagate |
    CWEventMask | CWOverrideRedirect | CWSaveUnder | CWWinGravity);
  window->attributes.background_pixel=pixel->background_color.pixel;
  window->attributes.background_pixmap=(Pixmap) NULL;
  window->attributes.bit_gravity=ForgetGravity;
  window->attributes.backing_store=WhenMapped;
  window->attributes.save_under=MagickTrue;
  window->attributes.border_pixel=pixel->border_color.pixel;
  window->attributes.colormap=map_info->colormap;
  window->attributes.cursor=window->cursor;
  window->attributes.do_not_propagate_mask=NoEventMask;
  window->attributes.event_mask=NoEventMask;
  window->attributes.override_redirect=MagickFalse;
  window->attributes.win_gravity=NorthWestGravity;
  window->orphan=MagickFalse;
}