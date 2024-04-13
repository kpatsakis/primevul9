static MagickBooleanType ClassifyImageColors(CubeInfo *cube_info,
  const Image *image,ExceptionInfo *exception)
{
#define ClassifyImageTag  "Classify/Image"

  CacheView
    *image_view;

  DoublePixelPacket
    error,
    mid,
    midpoint,
    pixel;

  MagickBooleanType
    proceed;

  MagickRealType
    bisect;

  NodeInfo
    *node_info;

  size_t
    count,
    id,
    index,
    level;

  ssize_t
    y;

  /*
    Classify the first cube_info->maximum_colors colors to a tree depth of 8.
  */
  SetAssociatedAlpha(image,cube_info);
  if ((cube_info->quantize_info->colorspace != UndefinedColorspace) &&
      (cube_info->quantize_info->colorspace != CMYKColorspace))
    (void) TransformImageColorspace((Image *) image,
      cube_info->quantize_info->colorspace);
  else
    if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
      (void) TransformImageColorspace((Image *) image,sRGBColorspace);
  midpoint.red=(MagickRealType) QuantumRange/2.0;
  midpoint.green=(MagickRealType) QuantumRange/2.0;
  midpoint.blue=(MagickRealType) QuantumRange/2.0;
  midpoint.opacity=(MagickRealType) QuantumRange/2.0;
  midpoint.index=(MagickRealType) QuantumRange/2.0;
  error.opacity=0.0;
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
    if (cube_info->nodes > MaxNodes)
      {
        /*
          Prune one level if the color tree is too large.
        */
        PruneLevel(cube_info,cube_info->root);
        cube_info->depth--;
      }
    for (x=0; x < (ssize_t) image->columns; x+=(ssize_t) count)
    {
      /*
        Start at the root and descend the color cube tree.
      */
      for (count=1; (x+(ssize_t) count) < (ssize_t) image->columns; count++)
        if (IsSameColor(image,p,p+count) == MagickFalse)
          break;
      AssociateAlphaPixel(cube_info,p,&pixel);
      index=MaxTreeDepth-1;
      bisect=((MagickRealType) QuantumRange+1.0)/2.0;
      mid=midpoint;
      node_info=cube_info->root;
      for (level=1; level <= MaxTreeDepth; level++)
      {
        double
          distance;

        bisect*=0.5;
        id=ColorToNodeId(cube_info,&pixel,index);
        mid.red+=(id & 1) != 0 ? bisect : -bisect;
        mid.green+=(id & 2) != 0 ? bisect : -bisect;
        mid.blue+=(id & 4) != 0 ? bisect : -bisect;
        mid.opacity+=(id & 8) != 0 ? bisect : -bisect;
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            /*
              Set colors of new node to contain pixel.
            */
            node_info->child[id]=GetNodeInfo(cube_info,id,level,node_info);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed","`%s'",
                  image->filename);
                continue;
              }
            if (level == MaxTreeDepth)
              cube_info->colors++;
          }
        /*
          Approximate the quantization error represented by this node.
        */
        node_info=node_info->child[id];
        error.red=QuantumScale*(pixel.red-mid.red);
        error.green=QuantumScale*(pixel.green-mid.green);
        error.blue=QuantumScale*(pixel.blue-mid.blue);
        if (cube_info->associate_alpha != MagickFalse)
          error.opacity=QuantumScale*(pixel.opacity-mid.opacity);
        distance=(double) (error.red*error.red+error.green*error.green+
          error.blue*error.blue+error.opacity*error.opacity);
        if (IsNaN(distance) != MagickFalse)
          distance=0.0;
        node_info->quantize_error+=count*sqrt(distance);
        cube_info->root->quantize_error+=node_info->quantize_error;
        index--;
      }
      /*
        Sum RGB for this leaf for later derivation of the mean cube color.
      */
      node_info->number_unique+=count;
      node_info->total_color.red+=count*QuantumScale*ClampPixel(pixel.red);
      node_info->total_color.green+=count*QuantumScale*ClampPixel(pixel.green);
      node_info->total_color.blue+=count*QuantumScale*ClampPixel(pixel.blue);
      if (cube_info->associate_alpha != MagickFalse)
        node_info->total_color.opacity+=count*QuantumScale*
          ClampPixel(pixel.opacity);
      else
        node_info->total_color.opacity+=count*QuantumScale*
          ClampPixel(OpaqueOpacity);
      p+=count;
    }
    if (cube_info->colors > cube_info->maximum_colors)
      {
        PruneToCubeDepth(cube_info,cube_info->root);
        break;
      }
    proceed=SetImageProgress(image,ClassifyImageTag,(MagickOffsetType) y,
      image->rows);
    if (proceed == MagickFalse)
      break;
  }
  for (y++; y < (ssize_t) image->rows; y++)
  {
    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (cube_info->nodes > MaxNodes)
      {
        /*
          Prune one level if the color tree is too large.
        */
        PruneLevel(cube_info,cube_info->root);
        cube_info->depth--;
      }
    for (x=0; x < (ssize_t) image->columns; x+=(ssize_t) count)
    {
      /*
        Start at the root and descend the color cube tree.
      */
      for (count=1; (x+(ssize_t) count) < (ssize_t) image->columns; count++)
        if (IsSameColor(image,p,p+count) == MagickFalse)
          break;
      AssociateAlphaPixel(cube_info,p,&pixel);
      index=MaxTreeDepth-1;
      bisect=((MagickRealType) QuantumRange+1.0)/2.0;
      mid=midpoint;
      node_info=cube_info->root;
      for (level=1; level <= cube_info->depth; level++)
      {
        double
          distance;

        bisect*=0.5;
        id=ColorToNodeId(cube_info,&pixel,index);
        mid.red+=(id & 1) != 0 ? bisect : -bisect;
        mid.green+=(id & 2) != 0 ? bisect : -bisect;
        mid.blue+=(id & 4) != 0 ? bisect : -bisect;
        mid.opacity+=(id & 8) != 0 ? bisect : -bisect;
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            /*
              Set colors of new node to contain pixel.
            */
            node_info->child[id]=GetNodeInfo(cube_info,id,level,node_info);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed","%s",
                  image->filename);
                  continue;
               }
            if (level == cube_info->depth)
              cube_info->colors++;
          }
        /*
          Approximate the quantization error represented by this node.
        */
        node_info=node_info->child[id];
        error.red=QuantumScale*(pixel.red-mid.red);
        error.green=QuantumScale*(pixel.green-mid.green);
        error.blue=QuantumScale*(pixel.blue-mid.blue);
        if (cube_info->associate_alpha != MagickFalse)
          error.opacity=QuantumScale*(pixel.opacity-mid.opacity);
        distance=(double) (error.red*error.red+error.green*error.green+
          error.blue*error.blue+error.opacity*error.opacity);
        if (IsNaN(distance))
          distance=0.0;
        node_info->quantize_error+=count*sqrt(distance);
        cube_info->root->quantize_error+=node_info->quantize_error;
        index--;
      }
      /*
        Sum RGB for this leaf for later derivation of the mean cube color.
      */
      node_info->number_unique+=count;
      node_info->total_color.red+=count*QuantumScale*ClampPixel(pixel.red);
      node_info->total_color.green+=count*QuantumScale*ClampPixel(pixel.green);
      node_info->total_color.blue+=count*QuantumScale*ClampPixel(pixel.blue);
      if (cube_info->associate_alpha != MagickFalse)
        node_info->total_color.opacity+=count*QuantumScale*ClampPixel(
          pixel.opacity);
      else
        node_info->total_color.opacity+=count*QuantumScale*
          ClampPixel(OpaqueOpacity);
      p+=count;
    }
    proceed=SetImageProgress(image,ClassifyImageTag,(MagickOffsetType) y,
      image->rows);
    if (proceed == MagickFalse)
      break;
  }
  image_view=DestroyCacheView(image_view);
  if ((cube_info->quantize_info->colorspace != UndefinedColorspace) &&
      (cube_info->quantize_info->colorspace != CMYKColorspace))
    (void) TransformImageColorspace((Image *) image,sRGBColorspace);
  return(y < (ssize_t) image->rows ? MagickFalse : MagickTrue);
}