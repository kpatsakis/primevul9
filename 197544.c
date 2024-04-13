MagickExport MagickBooleanType BilevelImageChannel(Image *image,
  const ChannelType channel,const double threshold)
{
#define ThresholdImageTag  "Threshold/Image"

  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  if (IsGrayColorspace(image->colorspace) != MagickFalse)
    (void) SetImageColorspace(image,sRGBColorspace);
  /*
    Bilevel threshold image.
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

    register ssize_t
      x;

    register PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    if ((channel & SyncChannels) != 0)
      {
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          SetPixelRed(q,GetPixelIntensity(image,q) <= threshold ? 0 :
            QuantumRange);
          SetPixelGreen(q,GetPixelRed(q));
          SetPixelBlue(q,GetPixelRed(q));
          q++;
        }
      }
    else
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        if ((channel & RedChannel) != 0)
          SetPixelRed(q,(MagickRealType) GetPixelRed(q) <= threshold ? 0 :
            QuantumRange);
        if ((channel & GreenChannel) != 0)
          SetPixelGreen(q,(MagickRealType) GetPixelGreen(q) <= threshold ? 0 :
            QuantumRange);
        if ((channel & BlueChannel) != 0)
          SetPixelBlue(q,(MagickRealType) GetPixelBlue(q) <= threshold ? 0 :
            QuantumRange);
        if ((channel & OpacityChannel) != 0)
          {
            if (image->matte == MagickFalse)
              SetPixelOpacity(q,(MagickRealType) GetPixelOpacity(q) <=
                threshold ? 0 : QuantumRange);
            else
              SetPixelAlpha(q,(MagickRealType) GetPixelAlpha(q) <= threshold ? 
                OpaqueOpacity : TransparentOpacity);
          }
        if (((channel & IndexChannel) != 0) &&
            (image->colorspace == CMYKColorspace))
          SetPixelIndex(indexes+x,(MagickRealType) GetPixelIndex(indexes+x) <=
            threshold ? 0 : QuantumRange);
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
        proceed=SetImageProgress(image,ThresholdImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}