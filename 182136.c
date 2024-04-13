MagickExport void XDelay(Display *display,const size_t milliseconds)
{
  assert(display != (Display *) NULL);
  (void) XFlush(display);
  MagickDelay(milliseconds);
}