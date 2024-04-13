static int MVGPrintf(DrawingWand *wand,const char *format,...)
{
  size_t
    extent;

  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",format);
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  extent=20UL*MagickPathExtent;
  if (wand->mvg == (char *) NULL)
    {
      wand->mvg=(char *) AcquireQuantumMemory(extent,sizeof(*wand->mvg));
      if (wand->mvg == (char *) NULL)
        {
          ThrowDrawException(ResourceLimitError,"MemoryAllocationFailed",
            wand->name);
          return(-1);
        }
      wand->mvg_alloc=extent;
      wand->mvg_length=0;
    }
  if (wand->mvg_alloc < (wand->mvg_length+10*MagickPathExtent))
    {
      extent+=wand->mvg_alloc;
      wand->mvg=(char *) ResizeQuantumMemory(wand->mvg,extent,
        sizeof(*wand->mvg));
      if (wand->mvg == (char *) NULL)
        {
          ThrowDrawException(ResourceLimitError,"MemoryAllocationFailed",
            wand->name);
          return(-1);
        }
      wand->mvg_alloc=extent;
    }
  {
    int
      count;

    ssize_t
      offset;

    va_list
      argp;

    while (wand->mvg_width < wand->indent_depth)
    {
      wand->mvg[wand->mvg_length]=' ';
      wand->mvg_length++;
      wand->mvg_width++;
    }
    wand->mvg[wand->mvg_length]='\0';
    count=(-1);
    offset=(ssize_t) wand->mvg_alloc-wand->mvg_length-1;
    if (offset > 0)
      {
        va_start(argp,format);
#if defined(MAGICKCORE_HAVE_VSNPRINTF)
        count=vsnprintf(wand->mvg+wand->mvg_length,(size_t) offset,format,argp);
#else
        count=vsprintf(wand->mvg+wand->mvg_length,format,argp);
#endif
        va_end(argp);
      }
    if ((count < 0) || (count > (int) offset))
      ThrowDrawException(DrawError,"UnableToPrint",format)
    else
      {
        wand->mvg_length+=count;
        wand->mvg_width+=count;
      }
    wand->mvg[wand->mvg_length]='\0';
    if ((wand->mvg_length > 1) && (wand->mvg[wand->mvg_length-1] == '\n'))
      wand->mvg_width=0;
    assert((wand->mvg_length+1) < wand->mvg_alloc);
    return(count);
  }
}