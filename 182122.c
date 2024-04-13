MagickExport void XWarning(const ExceptionType magick_unused(warning),
  const char *reason,const char *description)
{
  char
    text[MaxTextExtent];

  XWindows
    *windows;

  magick_unreferenced(warning);

  if (reason == (char *) NULL)
    return;
  (void) CopyMagickString(text,reason,MaxTextExtent);
  (void) ConcatenateMagickString(text,":",MaxTextExtent);
  windows=XSetWindows((XWindows *) ~0);
  XNoticeWidget(windows->display,windows,text,(char *) description);
}