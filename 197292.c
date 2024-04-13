MagickPrivate void XHighlightRectangle(Display *display,Window window,
  GC annotate_context,const RectangleInfo *highlight_info)
{
  assert(display != (Display *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(window != (Window) NULL);
  assert(annotate_context != (GC) NULL);
  assert(highlight_info != (RectangleInfo *) NULL);
  if ((highlight_info->width < 4) || (highlight_info->height < 4))
    return;
  (void) XDrawRectangle(display,window,annotate_context,(int) highlight_info->x,
    (int) highlight_info->y,(unsigned int) highlight_info->width-1,
    (unsigned int) highlight_info->height-1);
  (void) XDrawRectangle(display,window,annotate_context,(int) highlight_info->x+
    1,(int) highlight_info->y+1,(unsigned int) highlight_info->width-3,
    (unsigned int) highlight_info->height-3);
}