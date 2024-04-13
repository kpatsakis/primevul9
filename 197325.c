MagickPrivate void XConstrainWindowPosition(Display *display,
  XWindowInfo *window_info)
{
  int
    limit;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window_info != (XWindowInfo *) NULL);
  limit=XDisplayWidth(display,window_info->screen)-window_info->width;
  if (window_info->x < 0)
    window_info->x=0;
  else
    if (window_info->x > (int) limit)
      window_info->x=(int) limit;
  limit=XDisplayHeight(display,window_info->screen)-window_info->height;
  if (window_info->y < 0)
    window_info->y=0;
  else
    if (window_info->y > limit)
      window_info->y=limit;
}