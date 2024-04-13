MagickExport void XBestPixel(Display *display,const Colormap colormap,
  XColor *colors,unsigned int number_colors,XColor *color)
{
  MagickBooleanType
    query_server;

  MagickPixelPacket
    pixel;

  MagickRealType
    min_distance;

  register MagickRealType
    distance;

  register int
    i,
    j;

  Status
    status;

  /*
    Find closest representation for the requested RGB color.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(color != (XColor *) NULL);
  status=XAllocColor(display,colormap,color);
  if (status != False)
    return;
  query_server=colors == (XColor *) NULL ? MagickTrue : MagickFalse;
  if (query_server != MagickFalse)
    {
      /*
        Read X server colormap.
      */
      colors=(XColor *) AcquireQuantumMemory(number_colors,sizeof(*colors));
      if (colors == (XColor *) NULL)
        ThrowXWindowFatalException(ResourceLimitError,
          "MemoryAllocationFailed","...");
      for (i=0; i < (int) number_colors; i++)
        colors[i].pixel=(size_t) i;
      if (number_colors > 256)
        number_colors=256;
      (void) XQueryColors(display,colormap,colors,(int) number_colors);
    }
  min_distance=3.0*((MagickRealType) QuantumRange+1.0)*((MagickRealType)
    QuantumRange+1.0);
  j=0;
  for (i=0; i < (int) number_colors; i++)
  {
    pixel.red=colors[i].red-(MagickRealType) color->red;
    distance=pixel.red*pixel.red;
    if (distance > min_distance)
      continue;
    pixel.green=colors[i].green-(MagickRealType) color->green;
    distance+=pixel.green*pixel.green;
    if (distance > min_distance)
      continue;
    pixel.blue=colors[i].blue-(MagickRealType) color->blue;
    distance+=pixel.blue*pixel.blue;
    if (distance > min_distance)
      continue;
    min_distance=distance;
    color->pixel=colors[i].pixel;
    j=i;
  }
  (void) XAllocColor(display,colormap,&colors[j]);
  if (query_server != MagickFalse)
    colors=(XColor *) RelinquishMagickMemory(colors);
}