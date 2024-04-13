MagickExport MagickBooleanType PosterizeImageChannel(Image *image,
  const ChannelType channel,const size_t levels,const MagickBooleanType dither)
{
#define PosterizeImageTag  "Posterize/Image"
#define PosterizePixel(pixel) (Quantum) (QuantumRange*(MagickRound( \
  QuantumScale*pixel*(levels-1)))/MagickMax((ssize_t) levels-1,1))

  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  QuantizeInfo
    *quantize_info;

  register ssize_t
    i;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->storage_class == PseudoClass)
#if defined(MAGICKCORE_OPENMP_SUPPORT)
    #pragma omp parallel for schedule(static) shared(progress,status) \
      magick_number_threads(image,image,image->colors,1)
#endif
    for (i=0; i < (ssize_t) image->colors; i++)
    {
      /*
        Posterize colormap.
      */
      if ((channel & RedChannel) != 0)
        image->colormap[i].red=PosterizePixel(image->colormap[i].red);
      if ((channel & GreenChannel) != 0)
        image->colormap[i].green=PosterizePixel(image->colormap[i].green);
      if ((channel & BlueChannel) != 0)
        image->colormap[i].blue=PosterizePixel(image->colormap[i].blue);
      if ((channel & OpacityChannel) != 0)
        image->colormap[i].opacity=PosterizePixel(image->colormap[i].opacity);
    }
  /*
    Posterize image.
  */
  status=MagickTrue;
  progress=0;
  exception=(&image->exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register IndexPacket
      *magick_restrict indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((channel & RedChannel) != 0)
        SetPixelRed(q,PosterizePixel(GetPixelRed(q)));
      if ((channel & GreenChannel) != 0)
        SetPixelGreen(q,PosterizePixel(GetPixelGreen(q)));
      if ((channel & BlueChannel) != 0)
        SetPixelBlue(q,PosterizePixel(GetPixelBlue(q)));
      if (((channel & OpacityChannel) != 0) &&
          (image->matte != MagickFalse))
        SetPixelOpacity(q,PosterizePixel(GetPixelOpacity(q)));
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace))
        SetPixelIndex(indexes+x,PosterizePixel(GetPixelIndex(indexes+x)));
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,PosterizeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  quantize_info=AcquireQuantizeInfo((ImageInfo *) NULL);
  quantize_info->number_colors=(size_t) MagickMin((ssize_t) levels*levels*
    levels,MaxColormapSize+1);
  quantize_info->dither=dither;
  quantize_info->tree_depth=MaxTreeDepth;
  status=QuantizeImage(quantize_info,image);
  quantize_info=DestroyQuantizeInfo(quantize_info);
  return(status);
}