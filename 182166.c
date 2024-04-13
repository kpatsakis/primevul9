MagickExport MagickBooleanType XMagickProgressMonitor(const char *tag,
  const MagickOffsetType quantum,const MagickSizeType span,
  void *magick_unused(client_data))
{
  XWindows
    *windows;

  magick_unreferenced(client_data);

  windows=XSetWindows((XWindows *) ~0);
  if (windows == (XWindows *) NULL)
    return(MagickTrue);
  if (windows->info.mapped != MagickFalse)
    XProgressMonitorWidget(windows->display,windows,
      GetLocaleMonitorMessage(tag),quantum,span);
  return(MagickTrue);
}