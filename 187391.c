static MagickBooleanType AssignImageColors(Image *image,CubeInfo *cube_info)
{
#define AssignImageTag  "Assign/Image"

  ColorspaceType
    colorspace;

  ssize_t
    y;

  /*
    Allocate image colormap.
  */
  colorspace=image->colorspace;
  if (cube_info->quantize_info->colorspace != UndefinedColorspace)
    (void) TransformImageColorspace(image,cube_info->quantize_info->colorspace);
  if (AcquireImageColormap(image,cube_info->colors) == MagickFalse)
    ThrowBinaryImageException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  image->colors=0;
  cube_info->transparent_pixels=0;
  cube_info->transparent_index=(-1);
  (void) DefineImageColormap(image,cube_info,cube_info->root);
  /*
    Create a reduced color image.
  */
  if ((cube_info->quantize_info->dither != MagickFalse) &&
      (cube_info->quantize_info->dither_method != NoDitherMethod))
    (void) DitherImage(image,cube_info);
  else
    {
      CacheView
        *image_view;

      ExceptionInfo
        *exception;

      MagickBooleanType
        status;

      status=MagickTrue;
      exception=(&image->exception);
      image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static) shared(status) \
        magick_number_threads(image,image,image->rows,1)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        CubeInfo
          cube;

        register IndexPacket
          *magick_restrict indexes;

        register PixelPacket
          *magick_restrict q;

        register ssize_t
          x;

        ssize_t
          count;

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
        cube=(*cube_info);
        for (x=0; x < (ssize_t) image->columns; x+=count)
        {
          DoublePixelPacket
            pixel;

          register const NodeInfo
            *node_info;

          register ssize_t
            i;

          size_t
            id,
            index;

          /*
            Identify the deepest node containing the pixel's color.
          */
          for (count=1; (x+count) < (ssize_t) image->columns; count++)
            if (IsSameColor(image,q,q+count) == MagickFalse)
              break;
          AssociateAlphaPixel(&cube,q,&pixel);
          node_info=cube.root;
          for (index=MaxTreeDepth-1; (ssize_t) index > 0; index--)
          {
            id=ColorToNodeId(&cube,&pixel,index);
            if (node_info->child[id] == (NodeInfo *) NULL)
              break;
            node_info=node_info->child[id];
          }
          /*
            Find closest color among siblings and their children.
          */
          cube.target=pixel;
          cube.distance=(MagickRealType) (4.0*(QuantumRange+1.0)*
            (QuantumRange+1.0)+1.0);
          ClosestColor(image,&cube,node_info->parent);
          index=cube.color_number;
          for (i=0; i < (ssize_t) count; i++)
          {
            if (image->storage_class == PseudoClass)
              SetPixelIndex(indexes+x+i,index);
            if (cube.quantize_info->measure_error == MagickFalse)
              {
                SetPixelRgb(q,image->colormap+index);
                if (cube.associate_alpha != MagickFalse)
                  SetPixelOpacity(q,image->colormap[index].opacity);
              }
            q++;
          }
        }
        if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
          status=MagickFalse;
        if (image->progress_monitor != (MagickProgressMonitor) NULL)
          {
            MagickBooleanType
              proceed;

            proceed=SetImageProgress(image,AssignImageTag,(MagickOffsetType) y,
              image->rows);
            if (proceed == MagickFalse)
              status=MagickFalse;
          }
      }
      image_view=DestroyCacheView(image_view);
    }
  if (cube_info->quantize_info->measure_error != MagickFalse)
    (void) GetImageQuantizeError(image);
  if ((cube_info->quantize_info->number_colors == 2) &&
      ((cube_info->quantize_info->colorspace == LinearGRAYColorspace) ||
       (cube_info->quantize_info->colorspace == GRAYColorspace)))
    {
      double
        intensity;

      /*
        Monochrome image.
      */
      intensity=0.0;
      if ((image->colors > 1) &&
          (GetPixelLuma(image,image->colormap+0) > 
           GetPixelLuma(image,image->colormap+1)))
        intensity=(double) QuantumRange;
      image->colormap[0].red=intensity;
      image->colormap[0].green=intensity;
      image->colormap[0].blue=intensity;
      if (image->colors > 1)
        {
          image->colormap[1].red=(double) QuantumRange-intensity;
          image->colormap[1].green=(double) QuantumRange-intensity;
          image->colormap[1].blue=(double) QuantumRange-intensity;
        }
    }
  (void) SyncImage(image);
  if ((cube_info->quantize_info->colorspace != UndefinedColorspace) &&
      (IssRGBCompatibleColorspace(colorspace) == MagickFalse))
    (void) TransformImageColorspace(image,colorspace);
  return(MagickTrue);
}