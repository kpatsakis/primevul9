static MagickBooleanType RiemersmaDither(Image *image,CacheView *image_view,
  CubeInfo *cube_info,const unsigned int direction)
{
#define DitherImageTag  "Dither/Image"

  DoublePixelPacket
    color,
    pixel;

  MagickBooleanType
    proceed;

  register CubeInfo
    *p;

  size_t
    index;

  p=cube_info;
  if ((p->x >= 0) && (p->x < (ssize_t) image->columns) &&
      (p->y >= 0) && (p->y < (ssize_t) image->rows))
    {
      ExceptionInfo
        *exception;

      register IndexPacket
        *magick_restrict indexes;

      register PixelPacket
        *magick_restrict q;

      register ssize_t
        i;

      /*
        Distribute error.
      */
      exception=(&image->exception);
      q=GetCacheViewAuthenticPixels(image_view,p->x,p->y,1,1,exception);
      if (q == (PixelPacket *) NULL)
        return(MagickFalse);
      indexes=GetCacheViewAuthenticIndexQueue(image_view);
      AssociateAlphaPixel(cube_info,q,&pixel);
      for (i=0; i < ErrorQueueLength; i++)
      {
        pixel.red+=p->weights[i]*p->error[i].red;
        pixel.green+=p->weights[i]*p->error[i].green;
        pixel.blue+=p->weights[i]*p->error[i].blue;
        if (cube_info->associate_alpha != MagickFalse)
          pixel.opacity+=p->weights[i]*p->error[i].opacity;
      }
      pixel.red=(MagickRealType) ClampPixel(pixel.red);
      pixel.green=(MagickRealType) ClampPixel(pixel.green);
      pixel.blue=(MagickRealType) ClampPixel(pixel.blue);
      if (cube_info->associate_alpha != MagickFalse)
        pixel.opacity=(MagickRealType) ClampPixel(pixel.opacity);
      i=CacheOffset(cube_info,&pixel);
      if (p->cache[i] < 0)
        {
          register NodeInfo
            *node_info;

          register size_t
            id;

          /*
            Identify the deepest node containing the pixel's color.
          */
          node_info=p->root;
          for (index=MaxTreeDepth-1; (ssize_t) index > 0; index--)
          {
            id=ColorToNodeId(cube_info,&pixel,index);
            if (node_info->child[id] == (NodeInfo *) NULL)
              break;
            node_info=node_info->child[id];
          }
          /*
            Find closest color among siblings and their children.
          */
          p->target=pixel;
          p->distance=(MagickRealType) (4.0*(QuantumRange+1.0)*((MagickRealType)
            QuantumRange+1.0)+1.0);
          ClosestColor(image,p,node_info->parent);
          p->cache[i]=(ssize_t) p->color_number;
        }
      /*
        Assign pixel to closest colormap entry.
      */
      index=(size_t) (1*p->cache[i]);
      if (image->storage_class == PseudoClass)
        *indexes=(IndexPacket) index;
      if (cube_info->quantize_info->measure_error == MagickFalse)
        {
          SetPixelRgb(q,image->colormap+index);
          if (cube_info->associate_alpha != MagickFalse)
            SetPixelOpacity(q,image->colormap[index].opacity);
        }
      if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
        return(MagickFalse);
      /*
        Propagate the error as the last entry of the error queue.
      */
      (void) memmove(p->error,p->error+1,(ErrorQueueLength-1)*
        sizeof(p->error[0]));
      AssociateAlphaPixel(cube_info,image->colormap+index,&color);
      p->error[ErrorQueueLength-1].red=pixel.red-color.red;
      p->error[ErrorQueueLength-1].green=pixel.green-color.green;
      p->error[ErrorQueueLength-1].blue=pixel.blue-color.blue;
      if (cube_info->associate_alpha != MagickFalse)
        p->error[ErrorQueueLength-1].opacity=pixel.opacity-color.opacity;
      proceed=SetImageProgress(image,DitherImageTag,p->offset,p->span);
      if (proceed == MagickFalse)
        return(MagickFalse);
      p->offset++;
    }
  switch (direction)
  {
    case WestGravity: p->x--; break;
    case EastGravity: p->x++; break;
    case NorthGravity: p->y--; break;
    case SouthGravity: p->y++; break;
  }
  return(MagickTrue);
}