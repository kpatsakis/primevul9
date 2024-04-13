MagickExport MagickBooleanType GetImageQuantizeError(Image *image)
{
  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  IndexPacket
    *indexes;

  MagickRealType
    alpha,
    area,
    beta,
    distance,
    gamma,
    maximum_error,
    mean_error,
    mean_error_per_pixel;

  ssize_t
    index,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  (void) memset(&image->error,0,sizeof(image->error));
  if (image->storage_class == DirectClass)
    return(MagickTrue);
  alpha=1.0;
  beta=1.0;
  area=3.0*image->columns*image->rows;
  maximum_error=0.0;
  mean_error_per_pixel=0.0;
  mean_error=0.0;
  exception=(&image->exception);
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      index=(ssize_t) GetPixelIndex(indexes+x);
      if (image->matte != MagickFalse)
        {
          alpha=(MagickRealType) (QuantumScale*(GetPixelAlpha(p)));
          beta=(MagickRealType) (QuantumScale*(QuantumRange-
            image->colormap[index].opacity));
        }
      distance=fabs((double) (alpha*GetPixelRed(p)-beta*
        image->colormap[index].red));
      mean_error_per_pixel+=distance;
      mean_error+=distance*distance;
      if (distance > maximum_error)
        maximum_error=distance;
      distance=fabs((double) (alpha*GetPixelGreen(p)-beta*
        image->colormap[index].green));
      mean_error_per_pixel+=distance;
      mean_error+=distance*distance;
      if (distance > maximum_error)
        maximum_error=distance;
      distance=fabs((double) (alpha*GetPixelBlue(p)-beta*
        image->colormap[index].blue));
      mean_error_per_pixel+=distance;
      mean_error+=distance*distance;
      if (distance > maximum_error)
        maximum_error=distance;
      p++;
    }
  }
  image_view=DestroyCacheView(image_view);
  gamma=PerceptibleReciprocal(area);
  image->error.mean_error_per_pixel=gamma*mean_error_per_pixel;
  image->error.normalized_mean_error=gamma*QuantumScale*QuantumScale*mean_error;
  image->error.normalized_maximum_error=QuantumScale*maximum_error;
  return(MagickTrue);
}