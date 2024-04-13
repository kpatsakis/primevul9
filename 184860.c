static MagickBooleanType ApplyPSDOpacityMask(Image *image,const Image *mask,
  Quantum background,MagickBooleanType revert,ExceptionInfo *exception)
{
  Image
    *complete_mask;

  MagickBooleanType
    status;

  PixelInfo
    color;

  ssize_t
    y;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  applying opacity mask");
  complete_mask=CloneImage(image,image->columns,image->rows,MagickTrue,
    exception);
  if (complete_mask == (Image *) NULL)
    return(MagickFalse);
  complete_mask->alpha_trait=BlendPixelTrait;
  GetPixelInfo(complete_mask,&color);
  color.red=background;
  SetImageColor(complete_mask,&color,exception);
  status=CompositeImage(complete_mask,mask,OverCompositeOp,MagickTrue,
    mask->page.x-image->page.x,mask->page.y-image->page.y,exception);
  if (status == MagickFalse)
    {
      complete_mask=DestroyImage(complete_mask);
      return(status);
    }
  image->alpha_trait=BlendPixelTrait;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static,4) shared(status) \
  magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register Quantum
      *p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    p=GetAuthenticPixels(complete_mask,0,y,complete_mask->columns,1,exception);
    if ((q == (Quantum *) NULL) || (p == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      MagickRealType
        alpha,
        intensity;

      alpha=GetPixelAlpha(image,q);
      intensity=GetPixelIntensity(complete_mask,p);
      if (revert == MagickFalse)
        SetPixelAlpha(image,ClampToQuantum(intensity*(QuantumScale*alpha)),q);
      else if (intensity > 0)
        SetPixelAlpha(image,ClampToQuantum((alpha/intensity)*QuantumRange),q);
      q+=GetPixelChannels(image);
      p+=GetPixelChannels(complete_mask);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }
  complete_mask=DestroyImage(complete_mask);
  return(status);
}