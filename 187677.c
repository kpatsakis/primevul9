static MagickBooleanType DitherImage(Image *image,CubeInfo *cube_info)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  register ssize_t
    i;

  size_t
    depth;

  if (cube_info->quantize_info->dither_method != RiemersmaDitherMethod)
    return(FloydSteinbergDither(image,cube_info));
  /*
    Distribute quantization error along a Hilbert curve.
  */
  (void) memset(cube_info->error,0,ErrorQueueLength*
    sizeof(*cube_info->error));
  cube_info->x=0;
  cube_info->y=0;
  i=MagickMax((ssize_t) image->columns,(ssize_t) image->rows);
  for (depth=1; i != 0; depth++)
    i>>=1;
  if ((ssize_t) (1L << depth) < MagickMax((ssize_t) image->columns,(ssize_t) image->rows))
    depth++;
  cube_info->offset=0;
  cube_info->span=(MagickSizeType) image->columns*image->rows;
  image_view=AcquireAuthenticCacheView(image,&image->exception);
  if (depth > 1)
    Riemersma(image,image_view,cube_info,depth-1,NorthGravity);
  status=RiemersmaDither(image,image_view,cube_info,ForgetGravity);
  image_view=DestroyCacheView(image_view);
  return(status);
}