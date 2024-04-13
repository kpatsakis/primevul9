static MagickBooleanType CorrectPSDAlphaBlend(const ImageInfo *image_info,
  Image *image,ExceptionInfo* exception)
{
  const char
    *option;

  MagickBooleanType
    status;

  ssize_t
    y;

  if (image->alpha_trait != BlendPixelTrait || image->colorspace != sRGBColorspace)
    return(MagickTrue);
  option=GetImageOption(image_info,"psd:alpha-unblend");
  if (IsStringFalse(option) != MagickFalse)
    return(MagickTrue);
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static,4) shared(status) \
  magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
    {
      status=MagickFalse;
      continue;
    }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      double
        gamma;

      register ssize_t
        i;

      gamma=QuantumScale*GetPixelAlpha(image, q);
      if (gamma != 0.0 && gamma != 1.0)
        {
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel channel = GetPixelChannelChannel(image,i);
            if (channel != AlphaPixelChannel)
              q[i]=ClampToQuantum((q[i]-((1.0-gamma)*QuantumRange))/gamma);
          }
        }
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }

  return(status);
}