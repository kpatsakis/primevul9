MagickExport MagickBooleanType SigmoidalContrastImage(Image *image,
  const MagickBooleanType sharpen,const double contrast,const double midpoint,
  ExceptionInfo *exception)
{
#define SigmoidalContrastImageTag  "SigmoidalContrast/Image"
#define ScaledSig(x) ( ClampToQuantum(QuantumRange* \
  ScaledSigmoidal(contrast,QuantumScale*midpoint,QuantumScale*(x))) )
#define InverseScaledSig(x) ( ClampToQuantum(QuantumRange* \
  InverseScaledSigmoidal(contrast,QuantumScale*midpoint,QuantumScale*(x))) )

  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Convenience macros.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  /*
    Side effect: may clamp values unless contrast<MagickEpsilon, in which
    case nothing is done.
  */
  if (contrast < MagickEpsilon)
    return(MagickTrue);
  /*
    Sigmoidal-contrast enhance colormap.
  */
  if (image->storage_class == PseudoClass)
    {
      register ssize_t
        i;

      if( sharpen != MagickFalse )
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].red=(MagickRealType) ScaledSig(
              image->colormap[i].red);
          if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].green=(MagickRealType) ScaledSig(
              image->colormap[i].green);
          if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].blue=(MagickRealType) ScaledSig(
              image->colormap[i].blue);
          if ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].alpha=(MagickRealType) ScaledSig(
              image->colormap[i].alpha);
        }
      else
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].red=(MagickRealType) InverseScaledSig(
              image->colormap[i].red);
          if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].green=(MagickRealType) InverseScaledSig(
              image->colormap[i].green);
          if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].blue=(MagickRealType) InverseScaledSig(
              image->colormap[i].blue);
          if ((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0)
            image->colormap[i].alpha=(MagickRealType) InverseScaledSig(
              image->colormap[i].alpha);
        }
    }
  /*
    Sigmoidal-contrast enhance image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      if (GetPixelWriteMask(image,q) == 0)
        {
          q+=GetPixelChannels(image);
          continue;
        }
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel=GetPixelChannelChannel(image,i);
        PixelTrait traits=GetPixelChannelTraits(image,channel);
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        if( sharpen != MagickFalse )
          q[i]=ScaledSig(q[i]);
        else
          q[i]=InverseScaledSig(q[i]);
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_SigmoidalContrastImage)
#endif
        proceed=SetImageProgress(image,SigmoidalContrastImageTag,progress++,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}