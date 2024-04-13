MagickPrivate void XFreeResources(Display *display,XVisualInfo *visual_info,
  XStandardColormap *map_info,XPixelInfo *pixel,XFontStruct *font_info,
  XResourceInfo *resource_info,XWindowInfo *window_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  if (window_info != (XWindowInfo *) NULL)
    {
      /*
        Free X image.
      */
      if (window_info->ximage != (XImage *) NULL)
        XDestroyImage(window_info->ximage);
      if (window_info->id != (Window) NULL)
        {
          /*
            Free destroy window and free cursors.
          */
          if (window_info->id != XRootWindow(display,visual_info->screen))
            (void) XDestroyWindow(display,window_info->id);
          if (window_info->annotate_context != (GC) NULL)
            (void) XFreeGC(display,window_info->annotate_context);
          if (window_info->highlight_context != (GC) NULL)
            (void) XFreeGC(display,window_info->highlight_context);
          if (window_info->widget_context != (GC) NULL)
            (void) XFreeGC(display,window_info->widget_context);
          if (window_info->cursor != (Cursor) NULL)
            (void) XFreeCursor(display,window_info->cursor);
          window_info->cursor=(Cursor) NULL;
          if (window_info->busy_cursor != (Cursor) NULL)
            (void) XFreeCursor(display,window_info->busy_cursor);
          window_info->busy_cursor=(Cursor) NULL;
        }
    }
  /*
    Free font.
  */
  if (font_info != (XFontStruct *) NULL)
    {
      (void) XFreeFont(display,font_info);
      font_info=(XFontStruct *) NULL;
    }
  if (map_info != (XStandardColormap *) NULL)
    {
      /*
        Free X Standard Colormap.
      */
      if (resource_info->map_type == (char *) NULL)
        (void) XFreeStandardColormap(display,visual_info,map_info,pixel);
      (void) XFree((void *) map_info);
    }
  /*
    Free X visual info.
  */
  if (visual_info != (XVisualInfo *) NULL)
    (void) XFree((void *) visual_info);
  if (resource_info->close_server != MagickFalse)
    (void) XCloseDisplay(display);
}