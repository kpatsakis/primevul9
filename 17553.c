static MagickBooleanType FloydSteinbergDither(Image *image,CubeInfo *cube_info,
  ExceptionInfo *exception)
{
#define DitherImageTag  "Dither/Image"

  CacheView
    *image_view;

  const char
    *artifact;

  double
    amount;

  DoublePixelPacket
    **pixels;

  MagickBooleanType
    status;

  ssize_t
    y;

  /*
    Distribute quantization error using Floyd-Steinberg.
  */
  pixels=AcquirePixelThreadSet(image->columns);
  if (pixels == (DoublePixelPacket **) NULL)
    return(MagickFalse);
  status=MagickTrue;
  amount=1.0;
  artifact=GetImageArtifact(image,"dither:diffusion-amount");
  if (artifact != (const char *) NULL)
    amount=StringToDoubleInterval(artifact,1.0);
  image_view=AcquireAuthenticCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    CubeInfo
      cube;

    DoublePixelPacket
      *current,
      *previous;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    size_t
      index;

    ssize_t
      v;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    cube=(*cube_info);
    current=pixels[id]+(y & 0x01)*image->columns;
    previous=pixels[id]+((y+1) & 0x01)*image->columns;
    v=(ssize_t) ((y & 0x01) != 0 ? -1 : 1);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      DoublePixelPacket
        color,
        pixel;

      register ssize_t
        i;

      ssize_t
        u;

      u=(y & 0x01) != 0 ? (ssize_t) image->columns-1-x : x;
      AssociateAlphaPixel(image,&cube,q+u*GetPixelChannels(image),&pixel);
      if (x > 0)
        {
          pixel.red+=7.0*amount*current[u-v].red/16;
          pixel.green+=7.0*amount*current[u-v].green/16;
          pixel.blue+=7.0*amount*current[u-v].blue/16;
          if (cube.associate_alpha != MagickFalse)
            pixel.alpha+=7.0*amount*current[u-v].alpha/16;
        }
      if (y > 0)
        {
          if (x < (ssize_t) (image->columns-1))
            {
              pixel.red+=previous[u+v].red/16;
              pixel.green+=previous[u+v].green/16;
              pixel.blue+=previous[u+v].blue/16;
              if (cube.associate_alpha != MagickFalse)
                pixel.alpha+=previous[u+v].alpha/16;
            }
          pixel.red+=5.0*amount*previous[u].red/16;
          pixel.green+=5.0*amount*previous[u].green/16;
          pixel.blue+=5.0*amount*previous[u].blue/16;
          if (cube.associate_alpha != MagickFalse)
            pixel.alpha+=5.0*amount*previous[u].alpha/16;
          if (x > 0)
            {
              pixel.red+=3.0*amount*previous[u-v].red/16;
              pixel.green+=3.0*amount*previous[u-v].green/16;
              pixel.blue+=3.0*amount*previous[u-v].blue/16;
              if (cube.associate_alpha != MagickFalse)
                pixel.alpha+=3.0*amount*previous[u-v].alpha/16;
            }
        }
      pixel.red=(double) ClampPixel(pixel.red);
      pixel.green=(double) ClampPixel(pixel.green);
      pixel.blue=(double) ClampPixel(pixel.blue);
      if (cube.associate_alpha != MagickFalse)
        pixel.alpha=(double) ClampPixel(pixel.alpha);
      i=CacheOffset(&cube,&pixel);
      if (cube.cache[i] < 0)
        {
          register NodeInfo
            *node_info;

          register size_t
            node_id;

          /*
            Identify the deepest node containing the pixel's color.
          */
          node_info=cube.root;
          for (index=MaxTreeDepth-1; (ssize_t) index > 0; index--)
          {
            node_id=ColorToNodeId(&cube,&pixel,index);
            if (node_info->child[node_id] == (NodeInfo *) NULL)
              break;
            node_info=node_info->child[node_id];
          }
          /*
            Find closest color among siblings and their children.
          */
          cube.target=pixel;
          cube.distance=(double) (4.0*(QuantumRange+1.0)*(QuantumRange+1.0)+
            1.0);
          ClosestColor(image,&cube,node_info->parent);
          cube.cache[i]=(ssize_t) cube.color_number;
        }
      /*
        Assign pixel to closest colormap entry.
      */
      index=(size_t) cube.cache[i];
      if (image->storage_class == PseudoClass)
        SetPixelIndex(image,(Quantum) index,q+u*GetPixelChannels(image));
      if (cube.quantize_info->measure_error == MagickFalse)
        {
          SetPixelRed(image,ClampToQuantum(image->colormap[index].red),
            q+u*GetPixelChannels(image));
          SetPixelGreen(image,ClampToQuantum(image->colormap[index].green),
            q+u*GetPixelChannels(image));
          SetPixelBlue(image,ClampToQuantum(image->colormap[index].blue),
            q+u*GetPixelChannels(image));
          if (cube.associate_alpha != MagickFalse)
            SetPixelAlpha(image,ClampToQuantum(image->colormap[index].alpha),
              q+u*GetPixelChannels(image));
        }
      if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
        status=MagickFalse;
      /*
        Store the error.
      */
      AssociateAlphaPixelInfo(&cube,image->colormap+index,&color);
      current[u].red=pixel.red-color.red;
      current[u].green=pixel.green-color.green;
      current[u].blue=pixel.blue-color.blue;
      if (cube.associate_alpha != MagickFalse)
        current[u].alpha=pixel.alpha-color.alpha;
      if (image->progress_monitor != (MagickProgressMonitor) NULL)
        {
          MagickBooleanType
            proceed;

          proceed=SetImageProgress(image,DitherImageTag,(MagickOffsetType) y,
            image->rows);
          if (proceed == MagickFalse)
            status=MagickFalse;
        }
    }
  }
  image_view=DestroyCacheView(image_view);
  pixels=DestroyPixelThreadSet(pixels);
  return(MagickTrue);
}