MagickExport void XHighlightLine(Display *display,Window window,
  GC annotate_context,const XSegment *highlight_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(annotate_context != (GC) NULL);
  assert(highlight_info != (XSegment *) NULL);
  (void) XDrawLine(display,window,annotate_context,highlight_info->x1,
    highlight_info->y1,highlight_info->x2,highlight_info->y2);
}