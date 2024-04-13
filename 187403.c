MagickExport MagickBooleanType QuantizeImages(const QuantizeInfo *quantize_info,
  Image *images)
{
  CubeInfo
    *cube_info;

  Image
    *image;

  MagickBooleanType
    proceed,
    status;

  MagickProgressMonitor
    progress_monitor;

  register ssize_t
    i;

  size_t
    depth,
    maximum_colors,
    number_images;

  assert(quantize_info != (const QuantizeInfo *) NULL);
  assert(quantize_info->signature == MagickCoreSignature);
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  if (GetNextImageInList(images) == (Image *) NULL)
    {
      /*
        Handle a single image with QuantizeImage.
      */
      status=QuantizeImage(quantize_info,images);
      return(status);
    }
  status=MagickFalse;
  maximum_colors=quantize_info->number_colors;
  if (maximum_colors == 0)
    maximum_colors=MaxColormapSize;
  if (maximum_colors > MaxColormapSize)
    maximum_colors=MaxColormapSize;
  depth=quantize_info->tree_depth;
  if (depth == 0)
    {
      size_t
        colors;

      /*
        Depth of color tree is: Log4(colormap size)+2.
      */
      colors=maximum_colors;
      for (depth=1; colors != 0; depth++)
        colors>>=2;
      if (quantize_info->dither != MagickFalse)
        depth--;
    }
  /*
    Initialize color cube.
  */
  cube_info=GetCubeInfo(quantize_info,depth,maximum_colors);
  if (cube_info == (CubeInfo *) NULL)
    {
      (void) ThrowMagickException(&images->exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",images->filename);
      return(MagickFalse);
    }
  number_images=GetImageListLength(images);
  image=images;
  for (i=0; image != (Image *) NULL; i++)
  {
    progress_monitor=SetImageProgressMonitor(image,(MagickProgressMonitor) NULL,
      image->client_data);
    status=ClassifyImageColors(cube_info,image,&image->exception);
    if (status == MagickFalse)
      break;
    (void) SetImageProgressMonitor(image,progress_monitor,image->client_data);
    proceed=SetImageProgress(image,AssignImageTag,(MagickOffsetType) i,
      number_images);
    if (proceed == MagickFalse)
      break;
    image=GetNextImageInList(image);
  }
  if (status != MagickFalse)
    {
      /*
        Reduce the number of colors in an image sequence.
      */
      ReduceImageColors(images,cube_info);
      image=images;
      for (i=0; image != (Image *) NULL; i++)
      {
        progress_monitor=SetImageProgressMonitor(image,(MagickProgressMonitor)
          NULL,image->client_data);
        status=AssignImageColors(image,cube_info);
        if (status == MagickFalse)
          break;
        (void) SetImageProgressMonitor(image,progress_monitor,
          image->client_data);
        proceed=SetImageProgress(image,AssignImageTag,(MagickOffsetType) i,
          number_images);
        if (proceed == MagickFalse)
          break;
        image=GetNextImageInList(image);
      }
    }
  DestroyCubeInfo(cube_info);
  return(status);
}