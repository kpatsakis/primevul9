MagickPrivate void XWarning(const ExceptionType magick_unused(warning),
  const char *reason,const char *description)
{
  char
    text[MagickPathExtent];

  XWindows
    *windows;

  if (reason == (char *) NULL)
    return;
  (void) CopyMagickString(text,reason,MagickPathExtent);
  (void) ConcatenateMagickString(text,":",MagickPathExtent);
  windows=XSetWindows((XWindows *) ~0);
  XNoticeWidget(windows->display,windows,text,(char *) description);
}