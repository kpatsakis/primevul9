MagickExport MagickBooleanType RandomThresholdImageChannel(Image *image,
  const ChannelType channel,const char *thresholds,ExceptionInfo *exception)
{
#define ThresholdImageTag  "Threshold/Image"

  CacheView
    *image_view;

  GeometryInfo
    geometry_info;

  MagickStatusType
    flags;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    threshold;

  MagickRealType
    min_threshold,
    max_threshold;

  RandomInfo
    **magick_restrict random_info;

  ssize_t
    y;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
  unsigned long
    key;
#endif

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (thresholds == (const char *) NULL)
    return(MagickTrue);
  GetMagickPixelPacket(image,&threshold);
  min_threshold=0.0;
  max_threshold=(MagickRealType) QuantumRange;
  flags=ParseGeometry(thresholds,&geometry_info);
  min_threshold=geometry_info.rho;
  max_threshold=geometry_info.sigma;
  if ((flags & SigmaValue) == 0)
    max_threshold=min_threshold;
  if (strchr(thresholds,'%') != (char *) NULL)
    {
      max_threshold*=(MagickRealType) (0.01*QuantumRange);
      min_threshold*=(MagickRealType) (0.01*QuantumRange);
    }
  else
    if (((max_threshold == min_threshold) || (max_threshold == 1)) &&
        (min_threshold <= 8))
      {
        /*
          Backward Compatibility -- ordered-dither -- IM v 6.2.9-6.
        */
        status=OrderedPosterizeImageChannel(image,channel,thresholds,exception);
        return(status);
      }
  /*
    Random threshold image.
  */
  status=MagickTrue;
  progress=0;
  if (channel == CompositeChannels)
    {
      if (AcquireImageColormap(image,2) == MagickFalse)
        ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
          image->filename);
      random_info=AcquireRandomInfoThreadSet();
      image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      key=GetRandomSecretKey(random_info[0]);
      #pragma omp parallel for schedule(static) shared(progress,status) \
        magick_number_threads(image,image,image->rows,key == ~0UL)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        const int
          id = GetOpenMPThreadId();

        MagickBooleanType
          sync;

        register IndexPacket
          *magick_restrict indexes;

        register ssize_t
          x;

        register PixelPacket
          *magick_restrict q;

        if (status == MagickFalse)
          continue;
        q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
          exception);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        indexes=GetCacheViewAuthenticIndexQueue(image_view);
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          IndexPacket
            index;

          MagickRealType
            intensity;

          intensity=GetPixelIntensity(image,q);
          if (intensity < min_threshold)
            threshold.index=min_threshold;
          else if (intensity > max_threshold)
            threshold.index=max_threshold;
          else
            threshold.index=(MagickRealType)(QuantumRange*
              GetPseudoRandomValue(random_info[id]));
          index=(IndexPacket) (intensity <= threshold.index ? 0 : 1);
          SetPixelIndex(indexes+x,index);
          SetPixelRGBO(q,image->colormap+(ssize_t) index);
          q++;
        }
        sync=SyncCacheViewAuthenticPixels(image_view,exception);
        if (sync == MagickFalse)
          status=MagickFalse;
        if (image->progress_monitor != (MagickProgressMonitor) NULL)
          {
            MagickBooleanType
              proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
            #pragma omp atomic
#endif
            progress++;
            proceed=SetImageProgress(image,ThresholdImageTag,progress,
              image->rows);
            if (proceed == MagickFalse)
              status=MagickFalse;
          }
      }
      image_view=DestroyCacheView(image_view);
      random_info=DestroyRandomInfoThreadSet(random_info);
      return(status);
    }
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(MagickFalse);
    }
  random_info=AcquireRandomInfoThreadSet();
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  key=GetRandomSecretKey(random_info[0]);
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,key == ~0UL)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

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
        {
          if ((MagickRealType) GetPixelRed(q) < min_threshold)
            threshold.red=min_threshold;
          else
            if ((MagickRealType) GetPixelRed(q) > max_threshold)
              threshold.red=max_threshold;
            else
              threshold.red=(MagickRealType) (QuantumRange*
                GetPseudoRandomValue(random_info[id]));
        }
      if ((channel & GreenChannel) != 0)
        {
          if ((MagickRealType) GetPixelGreen(q) < min_threshold)
            threshold.green=min_threshold;
          else
            if ((MagickRealType) GetPixelGreen(q) > max_threshold)
              threshold.green=max_threshold;
            else
              threshold.green=(MagickRealType) (QuantumRange*
                GetPseudoRandomValue(random_info[id]));
        }
      if ((channel & BlueChannel) != 0)
        {
          if ((MagickRealType) GetPixelBlue(q) < min_threshold)
            threshold.blue=min_threshold;
          else
            if ((MagickRealType) GetPixelBlue(q) > max_threshold)
              threshold.blue=max_threshold;
            else
              threshold.blue=(MagickRealType) (QuantumRange*
                GetPseudoRandomValue(random_info[id]));
        }
      if ((channel & OpacityChannel) != 0)
        {
          if ((MagickRealType) GetPixelOpacity(q) < min_threshold)
            threshold.opacity=min_threshold;
          else
            if ((MagickRealType) GetPixelOpacity(q) > max_threshold)
              threshold.opacity=max_threshold;
            else
              threshold.opacity=(MagickRealType) (QuantumRange*
                GetPseudoRandomValue(random_info[id]));
        }
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace))
        {
          if ((MagickRealType) GetPixelIndex(indexes+x) < min_threshold)
            threshold.index=min_threshold;
          else
            if ((MagickRealType) GetPixelIndex(indexes+x) > max_threshold)
              threshold.index=max_threshold;
            else
              threshold.index=(MagickRealType) (QuantumRange*
                GetPseudoRandomValue(random_info[id]));
        }
      if ((channel & RedChannel) != 0)
        SetPixelRed(q,(MagickRealType) GetPixelRed(q) <= threshold.red ?
          0 : QuantumRange);
      if ((channel & GreenChannel) != 0)
        SetPixelGreen(q,(MagickRealType) GetPixelGreen(q) <= threshold.green ?
          0 : QuantumRange);
      if ((channel & BlueChannel) != 0)
        SetPixelBlue(q,(MagickRealType) GetPixelBlue(q) <= threshold.blue ?
          0 : QuantumRange);
      if ((channel & OpacityChannel) != 0)
        SetPixelOpacity(q,(MagickRealType) GetPixelOpacity(q) <=
          threshold.opacity ? 0 : QuantumRange);
      if (((channel & IndexChannel) != 0) &&
          (image->colorspace == CMYKColorspace))
        SetPixelIndex(indexes+x,(MagickRealType) GetPixelIndex(indexes+x) <=
          threshold.index ? 0 : QuantumRange);
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
  random_info=DestroyRandomInfoThreadSet(random_info);
  return(status);
}