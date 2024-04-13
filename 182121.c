MagickExport XWindows *XSetWindows(XWindows *windows_info)
{
  static XWindows
    *windows = (XWindows *) NULL;

  if (windows_info != (XWindows *) ~0)
    {
      windows=(XWindows *) RelinquishMagickMemory(windows);
      windows=windows_info;
    }
  return(windows);
}