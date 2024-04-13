static int MVGAutoWrapPrintf(DrawingWand *wand,const char *format,...)
{
  char
    buffer[MagickPathExtent];

  int
    count;

  va_list
    argp;

  va_start(argp,format);
#if defined(MAGICKCORE_HAVE_VSNPRINTF)
  count=vsnprintf(buffer,sizeof(buffer)-1,format,argp);
#else
  count=vsprintf(buffer,format,argp);
#endif
  va_end(argp);
  buffer[sizeof(buffer)-1]='\0';
  if (count < 0)
    ThrowDrawException(DrawError,"UnableToPrint",format)
  else
    {
      if (((wand->mvg_width + count) > 78) && (buffer[count-1] != '\n'))
        (void) MVGPrintf(wand, "\n");
      (void) MVGPrintf(wand,"%s",buffer);
    }
  return(count);
}