WandExport MagickBooleanType MogrifyImageList(ImageInfo *image_info,
  const int argc,const char **argv,Image **images,ExceptionInfo *exception)
{
  const char
    *option;

  ImageInfo
    *mogrify_info;

  MagickStatusType
    status;

  PixelInterpolateMethod
   interpolate_method;

  QuantizeInfo
    *quantize_info;

  register ssize_t
    i;

  ssize_t
    count,
    index;

  /*
    Apply options to the image list.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(images != (Image **) NULL);
  assert((*images)->previous == (Image *) NULL);
  assert((*images)->signature == MagickCoreSignature);
  if ((*images)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      (*images)->filename);
  if ((argc <= 0) || (*argv == (char *) NULL))
    return(MagickTrue);
  interpolate_method=UndefinedInterpolatePixel;
  mogrify_info=CloneImageInfo(image_info);
  quantize_info=AcquireQuantizeInfo(mogrify_info);
  status=MagickTrue;
  for (i=0; i < (ssize_t) argc; i++)
  {
    if (*images == (Image *) NULL)
      break;
    option=argv[i];
    if (IsCommandOption(option) == MagickFalse)
      continue;
    count=ParseCommandOption(MagickCommandOptions,MagickFalse,option);
    count=MagickMax(count,0L);
    if ((i+count) >= (ssize_t) argc)
      break;
    status=MogrifyImageInfo(mogrify_info,(int) count+1,argv+i,exception);
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affinity",option+1) == 0)
          {
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            if (*option == '+')
              {
                (void) RemapImages(quantize_info,*images,(Image *) NULL,
                  exception);
                break;
              }
            i++;
            break;
          }
        if (LocaleCompare("append",option+1) == 0)
          {
            Image
              *append_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            append_image=AppendImages(*images,*option == '-' ? MagickTrue :
              MagickFalse,exception);
            if (append_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=append_image;
            break;
          }
        if (LocaleCompare("average",option+1) == 0)
          {
            Image
              *average_image;

            /*
              Average an image sequence (deprecated).
            */
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            average_image=EvaluateImages(*images,MeanEvaluateOperator,
              exception);
            if (average_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=average_image;
            break;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("channel-fx",option+1) == 0)
          {
            Image
              *channel_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            channel_image=ChannelFxImage(*images,argv[i+1],exception);
            if (channel_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=channel_image;
            break;
          }
        if (LocaleCompare("clut",option+1) == 0)
          {
            Image
              *clut_image,
              *image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            image=RemoveFirstImageFromList(images);
            clut_image=RemoveFirstImageFromList(images);
            if (clut_image == (Image *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"ImageSequenceRequired","`%s'",option);
                image=DestroyImage(image);
                status=MagickFalse;
                break;
              }
            (void) ClutImage(image,clut_image,interpolate_method,exception);
            clut_image=DestroyImage(clut_image);
            *images=DestroyImageList(*images);
            *images=image;
            break;
          }
        if (LocaleCompare("coalesce",option+1) == 0)
          {
            Image
              *coalesce_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            coalesce_image=CoalesceImages(*images,exception);
            if (coalesce_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=coalesce_image;
            break;
          }
        if (LocaleCompare("combine",option+1) == 0)
          {
            ColorspaceType
              colorspace;

            Image
              *combine_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            colorspace=(*images)->colorspace;
            if ((*images)->number_channels < GetImageListLength(*images))
              colorspace=sRGBColorspace;
            if (*option == '+')
              colorspace=(ColorspaceType) ParseCommandOption(
                MagickColorspaceOptions,MagickFalse,argv[i+1]);
            combine_image=CombineImages(*images,colorspace,exception);
            if (combine_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=combine_image;
            break;
          }
        if (LocaleCompare("compare",option+1) == 0)
          {
            double
              distortion;

            Image
              *difference_image,
              *image,
              *reconstruct_image;

            MetricType
              metric;

            /*
              Mathematically and visually annotate the difference between an
              image and its reconstruction.
            */
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            image=RemoveFirstImageFromList(images);
            reconstruct_image=RemoveFirstImageFromList(images);
            if (reconstruct_image == (Image *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"ImageSequenceRequired","`%s'",option);
                image=DestroyImage(image);
                status=MagickFalse;
                break;
              }
            metric=UndefinedErrorMetric;
            option=GetImageOption(mogrify_info,"metric");
            if (option != (const char *) NULL)
              metric=(MetricType) ParseCommandOption(MagickMetricOptions,
                MagickFalse,option);
            difference_image=CompareImages(image,reconstruct_image,metric,
              &distortion,exception);
            if (difference_image == (Image *) NULL)
              break;
            reconstruct_image=DestroyImage(reconstruct_image);
            image=DestroyImage(image);
            if (*images != (Image *) NULL)
              *images=DestroyImageList(*images);
            *images=difference_image;
            break;
          }
        if (LocaleCompare("complex",option+1) == 0)
          {
            ComplexOperator
              op;

            Image
              *complex_images;

            (void) SyncImageSettings(mogrify_info,*images,exception);
            op=(ComplexOperator) ParseCommandOption(MagickComplexOptions,
              MagickFalse,argv[i+1]);
            complex_images=ComplexImages(*images,op,exception);
            if (complex_images == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=complex_images;
            break;
          }
        if (LocaleCompare("composite",option+1) == 0)
          {
            CompositeOperator
              compose;

            const char*
              value;

            MagickBooleanType
              clip_to_self;

            Image
              *mask_image,
              *new_images,
              *source_image;

            RectangleInfo
              geometry;

            /* Compose value from "-compose" option only */
            (void) SyncImageSettings(mogrify_info,*images,exception);
            value=GetImageOption(mogrify_info,"compose");
            if (value == (const char *) NULL)
              compose=OverCompositeOp;  /* use Over not source_image->compose */
            else
              compose=(CompositeOperator) ParseCommandOption(
                MagickComposeOptions,MagickFalse,value);

            /* Get "clip-to-self" expert setting (false is normal) */
            clip_to_self=GetCompositeClipToSelf(compose);
            value=GetImageOption(mogrify_info,"compose:clip-to-self");
            if (value != (const char *) NULL)
              clip_to_self=IsStringTrue(value);
            value=GetImageOption(mogrify_info,"compose:outside-overlay");
            if (value != (const char *) NULL)
              clip_to_self=IsStringFalse(value);  /* deprecated */

            new_images=RemoveFirstImageFromList(images);
            source_image=RemoveFirstImageFromList(images);
            if (source_image == (Image *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"ImageSequenceRequired","`%s'",option);
                new_images=DestroyImage(new_images);
                status=MagickFalse;
                break;
              }

            /* FUTURE: this should not be here! - should be part of -geometry */
            if (source_image->geometry != (char *) NULL)
              {
                RectangleInfo
                  resize_geometry;

                (void) ParseRegionGeometry(source_image,source_image->geometry,
                  &resize_geometry,exception);
                if ((source_image->columns != resize_geometry.width) ||
                    (source_image->rows != resize_geometry.height))
                  {
                    Image
                      *resize_image;

                    resize_image=ResizeImage(source_image,resize_geometry.width,
                      resize_geometry.height,source_image->filter,exception);
                    if (resize_image != (Image *) NULL)
                      {
                        source_image=DestroyImage(source_image);
                        source_image=resize_image;
                      }
                  }
              }
            SetGeometry(source_image,&geometry);
            (void) ParseAbsoluteGeometry(source_image->geometry,&geometry);
            GravityAdjustGeometry(new_images->columns,new_images->rows,
              new_images->gravity,&geometry);
            mask_image=RemoveFirstImageFromList(images);
            if (mask_image == (Image *) NULL)
              status&=CompositeImage(new_images,source_image,compose,
                clip_to_self,geometry.x,geometry.y,exception);
            else
              {
                if ((compose == DisplaceCompositeOp) ||
                    (compose == DistortCompositeOp))
                  {
                    status&=CompositeImage(source_image,mask_image,
                      CopyGreenCompositeOp,MagickTrue,0,0,exception);
                    status&=CompositeImage(new_images,source_image,compose,
                      clip_to_self,geometry.x,geometry.y,exception);
                  }
                else
                  {
                    Image
                      *clone_image;

                    clone_image=CloneImage(new_images,0,0,MagickTrue,exception);
                    if (clone_image == (Image *) NULL)
                      break;
                    status&=CompositeImage(new_images,source_image,compose,
                      clip_to_self,geometry.x,geometry.y,exception);
                    status&=CompositeImage(new_images,mask_image,
                      CopyAlphaCompositeOp,MagickTrue,0,0,exception);
                    status&=CompositeImage(clone_image,new_images,
                      OverCompositeOp,clip_to_self,0,0,exception);
                    new_images=DestroyImageList(new_images);
                    new_images=clone_image;
                  }
                mask_image=DestroyImage(mask_image);
              }
            source_image=DestroyImage(source_image);
            *images=DestroyImageList(*images);
            *images=new_images;
            break;
          }
        if (LocaleCompare("copy",option+1) == 0)
          {
            Image
              *source_image;

            OffsetInfo
              offset;

            RectangleInfo
              geometry;

            /*
              Copy image pixels.
            */
            (void) SyncImageSettings(mogrify_info,*images,exception);
            (void) ParsePageGeometry(*images,argv[i+2],&geometry,exception);
            offset.x=geometry.x;
            offset.y=geometry.y;
            source_image=(*images);
            if (source_image->next != (Image *) NULL)
              source_image=source_image->next;
            (void) ParsePageGeometry(source_image,argv[i+1],&geometry,
              exception);
            status=CopyImagePixels(*images,source_image,&geometry,&offset,
              exception);
            break;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("deconstruct",option+1) == 0)
          {
            Image
              *deconstruct_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            deconstruct_image=CompareImagesLayers(*images,CompareAnyLayer,
              exception);
            if (deconstruct_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=deconstruct_image;
            break;
          }
        if (LocaleCompare("delete",option+1) == 0)
          {
            if (*option == '+')
              DeleteImages(images,"-1",exception);
            else
              DeleteImages(images,argv[i+1],exception);
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            if (*option == '+')
              {
                quantize_info->dither_method=NoDitherMethod;
                break;
              }
            quantize_info->dither_method=(DitherMethod) ParseCommandOption(
              MagickDitherOptions,MagickFalse,argv[i+1]);
            break;
          }
        if (LocaleCompare("duplicate",option+1) == 0)
          {
            Image
              *duplicate_images;

            if (*option == '+')
              duplicate_images=DuplicateImages(*images,1,"-1",exception);
            else
              {
                const char
                  *p;

                size_t
                  number_duplicates;

                number_duplicates=(size_t) StringToLong(argv[i+1]);
                p=strchr(argv[i+1],',');
                if (p == (const char *) NULL)
                  duplicate_images=DuplicateImages(*images,number_duplicates,
                    "-1",exception);
                else
                  duplicate_images=DuplicateImages(*images,number_duplicates,p,
                    exception);
              }
            AppendImageToList(images, duplicate_images);
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            break;
          }
        break;
      }
      case 'e':
      {
        if (LocaleCompare("evaluate-sequence",option+1) == 0)
          {
            Image
              *evaluate_image;

            MagickEvaluateOperator
              op;

            (void) SyncImageSettings(mogrify_info,*images,exception);
            op=(MagickEvaluateOperator) ParseCommandOption(
              MagickEvaluateOptions,MagickFalse,argv[i+1]);
            evaluate_image=EvaluateImages(*images,op,exception);
            if (evaluate_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=evaluate_image;
            break;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("fft",option+1) == 0)
          {
            Image
              *fourier_image;

            /*
              Implements the discrete Fourier transform (DFT).
            */
            (void) SyncImageSettings(mogrify_info,*images,exception);
            fourier_image=ForwardFourierTransformImage(*images,*option == '-' ?
              MagickTrue : MagickFalse,exception);
            if (fourier_image == (Image *) NULL)
              break;
            *images=DestroyImageList(*images);
            *images=fourier_image;
            break;
          }
        if (LocaleCompare("flatten",option+1) == 0)
          {
            Image
              *flatten_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            flatten_image=MergeImageLayers(*images,FlattenLayer,exception);
            if (flatten_image == (Image *) NULL)
              break;
            *images=DestroyImageList(*images);
            *images=flatten_image;
            break;
          }
        if (LocaleCompare("fx",option+1) == 0)
          {
            Image
              *fx_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            fx_image=FxImage(*images,argv[i+1],exception);
            if (fx_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=fx_image;
            break;
          }
        break;
      }
      case 'h':
      {
        if (LocaleCompare("hald-clut",option+1) == 0)
          {
            Image
              *hald_image,
              *image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            image=RemoveFirstImageFromList(images);
            hald_image=RemoveFirstImageFromList(images);
            if (hald_image == (Image *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"ImageSequenceRequired","`%s'",option);
                image=DestroyImage(image);
                status=MagickFalse;
                break;
              }
            (void) HaldClutImage(image,hald_image,exception);
            hald_image=DestroyImage(hald_image);
            if (*images != (Image *) NULL)
              *images=DestroyImageList(*images);
            *images=image;
            break;
          }
        break;
      }
      case 'i':
      {
        if (LocaleCompare("ift",option+1) == 0)
          {
            Image
              *fourier_image,
              *magnitude_image,
              *phase_image;

            /*
              Implements the inverse fourier discrete Fourier transform (DFT).
            */
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            magnitude_image=RemoveFirstImageFromList(images);
            phase_image=RemoveFirstImageFromList(images);
            if (phase_image == (Image *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"ImageSequenceRequired","`%s'",option);
                magnitude_image=DestroyImage(magnitude_image);
                status=MagickFalse;
                break;
              }
            fourier_image=InverseFourierTransformImage(magnitude_image,
              phase_image,*option == '-' ? MagickTrue : MagickFalse,exception);
            magnitude_image=DestroyImage(magnitude_image);
            phase_image=DestroyImage(phase_image);
            if (fourier_image == (Image *) NULL)
              break;
            if (*images != (Image *) NULL)
              *images=DestroyImageList(*images);
            *images=fourier_image;
            break;
          }
        if (LocaleCompare("insert",option+1) == 0)
          {
            Image
              *p,
              *q;

            index=0;
            if (*option != '+')
              index=(ssize_t) StringToLong(argv[i+1]);
            p=RemoveLastImageFromList(images);
            if (p == (Image *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"NoSuchImage","`%s'",argv[i+1]);
                status=MagickFalse;
                break;
              }
            q=p;
            if (index == 0)
              PrependImageToList(images,q);
            else
              if (index == (ssize_t) GetImageListLength(*images))
                AppendImageToList(images,q);
              else
                {
                   q=GetImageFromList(*images,index-1);
                   if (q == (Image *) NULL)
                     {
                       p=DestroyImage(p);
                       (void) ThrowMagickException(exception,GetMagickModule(),
                         OptionError,"NoSuchImage","`%s'",argv[i+1]);
                       status=MagickFalse;
                       break;
                     }
                  InsertImageInList(&q,p);
                }
            *images=GetFirstImageInList(q);
            break;
          }
        if (LocaleCompare("interpolate",option+1) == 0)
          {
            interpolate_method=(PixelInterpolateMethod) ParseCommandOption(
              MagickInterpolateOptions,MagickFalse,argv[i+1]);
            break;
          }
        break;
      }
      case 'l':
      {
        if (LocaleCompare("layers",option+1) == 0)
          {
            Image
              *layers;

            LayerMethod
              method;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            layers=(Image *) NULL;
            method=(LayerMethod) ParseCommandOption(MagickLayerOptions,
              MagickFalse,argv[i+1]);
            switch (method)
            {
              case CoalesceLayer:
              {
                layers=CoalesceImages(*images,exception);
                break;
              }
              case CompareAnyLayer:
              case CompareClearLayer:
              case CompareOverlayLayer:
              default:
              {
                layers=CompareImagesLayers(*images,method,exception);
                break;
              }
              case MergeLayer:
              case FlattenLayer:
              case MosaicLayer:
              case TrimBoundsLayer:
              {
                layers=MergeImageLayers(*images,method,exception);
                break;
              }
              case DisposeLayer:
              {
                layers=DisposeImages(*images,exception);
                break;
              }
              case OptimizeImageLayer:
              {
                layers=OptimizeImageLayers(*images,exception);
                break;
              }
              case OptimizePlusLayer:
              {
                layers=OptimizePlusImageLayers(*images,exception);
                break;
              }
              case OptimizeTransLayer:
              {
                OptimizeImageTransparency(*images,exception);
                break;
              }
              case RemoveDupsLayer:
              {
                RemoveDuplicateLayers(images,exception);
                break;
              }
              case RemoveZeroLayer:
              {
                RemoveZeroDelayLayers(images,exception);
                break;
              }
              case OptimizeLayer:
              {
                /*
                  General Purpose, GIF Animation Optimizer.
                */
                layers=CoalesceImages(*images,exception);
                if (layers == (Image *) NULL)
                  {
                    status=MagickFalse;
                    break;
                  }
                *images=DestroyImageList(*images);
                *images=layers;
                layers=OptimizeImageLayers(*images,exception);
                if (layers == (Image *) NULL)
                  {
                    status=MagickFalse;
                    break;
                  }
                *images=DestroyImageList(*images);
                *images=layers;
                layers=(Image *) NULL;
                OptimizeImageTransparency(*images,exception);
                (void) RemapImages(quantize_info,*images,(Image *) NULL,
                  exception);
                break;
              }
              case CompositeLayer:
              {
                CompositeOperator
                  compose;

                Image
                  *source;

                RectangleInfo
                  geometry;

                /*
                  Split image sequence at the first 'NULL:' image.
                */
                source=(*images);
                while (source != (Image *) NULL)
                {
                  source=GetNextImageInList(source);
                  if ((source != (Image *) NULL) &&
                      (LocaleCompare(source->magick,"NULL") == 0))
                    break;
                }
                if (source != (Image *) NULL)
                  {
                    if ((GetPreviousImageInList(source) == (Image *) NULL) ||
                        (GetNextImageInList(source) == (Image *) NULL))
                      source=(Image *) NULL;
                    else
                      {
                        /*
                          Separate the two lists, junk the null: image.
                        */
                        source=SplitImageList(source->previous);
                        DeleteImageFromList(&source);
                      }
                  }
                if (source == (Image *) NULL)
                  {
                    (void) ThrowMagickException(exception,GetMagickModule(),
                      OptionError,"MissingNullSeparator","layers Composite");
                    status=MagickFalse;
                    break;
                  }
                /*
                  Adjust offset with gravity and virtual canvas.
                */
                SetGeometry(*images,&geometry);
                (void) ParseAbsoluteGeometry((*images)->geometry,&geometry);
                geometry.width=source->page.width != 0 ?
                  source->page.width : source->columns;
                geometry.height=source->page.height != 0 ?
                 source->page.height : source->rows;
                GravityAdjustGeometry((*images)->page.width != 0 ?
                  (*images)->page.width : (*images)->columns,
                  (*images)->page.height != 0 ? (*images)->page.height :
                  (*images)->rows,(*images)->gravity,&geometry);
                compose=OverCompositeOp;
                option=GetImageOption(mogrify_info,"compose");
                if (option != (const char *) NULL)
                  compose=(CompositeOperator) ParseCommandOption(
                    MagickComposeOptions,MagickFalse,option);
                CompositeLayers(*images,compose,source,geometry.x,geometry.y,
                  exception);
                source=DestroyImageList(source);
                break;
              }
            }
            if (layers == (Image *) NULL)
              break;
            *images=DestroyImageList(*images);
            *images=layers;
            break;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            if (*option == '+')
              {
                (void) RemapImages(quantize_info,*images,(Image *) NULL,
                  exception);
                break;
              }
            i++;
            break;
          }
        if (LocaleCompare("maximum",option+1) == 0)
          {
            Image
              *maximum_image;

            /*
              Maximum image sequence (deprecated).
            */
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            maximum_image=EvaluateImages(*images,MaxEvaluateOperator,exception);
            if (maximum_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=maximum_image;
            break;
          }
        if (LocaleCompare("minimum",option+1) == 0)
          {
            Image
              *minimum_image;

            /*
              Minimum image sequence (deprecated).
            */
            (void) SyncImagesSettings(mogrify_info,*images,exception);
            minimum_image=EvaluateImages(*images,MinEvaluateOperator,exception);
            if (minimum_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=minimum_image;
            break;
          }
        if (LocaleCompare("morph",option+1) == 0)
          {
            Image
              *morph_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            morph_image=MorphImages(*images,StringToUnsignedLong(argv[i+1]),
              exception);
            if (morph_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=morph_image;
            break;
          }
        if (LocaleCompare("mosaic",option+1) == 0)
          {
            Image
              *mosaic_image;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            mosaic_image=MergeImageLayers(*images,MosaicLayer,exception);
            if (mosaic_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=mosaic_image;
            break;
          }
        break;
      }
      case 'p':
      {
        if (LocaleCompare("poly",option+1) == 0)
          {
            char
              *args,
              token[MagickPathExtent];

            const char
              *p;

            double
              *arguments;

            Image
              *polynomial_image;

            register ssize_t
              x;

            size_t
              number_arguments;

            /*
              Polynomial image.
            */
            (void) SyncImageSettings(mogrify_info,*images,exception);
            args=InterpretImageProperties(mogrify_info,*images,argv[i+1],
              exception);
            if (args == (char *) NULL)
              break;
            p=(char *) args;
            for (x=0; *p != '\0'; x++)
            {
              GetNextToken(p,&p,MagickPathExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MagickPathExtent,token);
            }
            number_arguments=(size_t) x;
            arguments=(double *) AcquireQuantumMemory(number_arguments,
              sizeof(*arguments));
            if (arguments == (double *) NULL)
              ThrowWandFatalException(ResourceLimitFatalError,
                "MemoryAllocationFailed",(*images)->filename);
            (void) memset(arguments,0,number_arguments*
              sizeof(*arguments));
            p=(char *) args;
            for (x=0; (x < (ssize_t) number_arguments) && (*p != '\0'); x++)
            {
              GetNextToken(p,&p,MagickPathExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MagickPathExtent,token);
              arguments[x]=StringToDouble(token,(char **) NULL);
            }
            args=DestroyString(args);
            polynomial_image=PolynomialImage(*images,number_arguments >> 1,
              arguments,exception);
            arguments=(double *) RelinquishMagickMemory(arguments);
            if (polynomial_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=polynomial_image;
          }
        if (LocaleCompare("print",option+1) == 0)
          {
            char
              *string;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            string=InterpretImageProperties(mogrify_info,*images,argv[i+1],
              exception);
            if (string == (char *) NULL)
              break;
            (void) FormatLocaleFile(stdout,"%s",string);
            string=DestroyString(string);
          }
        if (LocaleCompare("process",option+1) == 0)
          {
            char
              **arguments;

            int
              j,
              number_arguments;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            arguments=StringToArgv(argv[i+1],&number_arguments);
            if (arguments == (char **) NULL)
              break;
            if ((argc > 1) && (strchr(arguments[1],'=') != (char *) NULL))
              {
                char
                  breaker,
                  quote,
                  *token;

                const char
                  *argument;

                int
                  next,
                  token_status;

                size_t
                  length;

                TokenInfo
                  *token_info;

                /*
                  Support old style syntax, filter="-option arg".
                */
                length=strlen(argv[i+1]);
                token=(char *) NULL;
                if (~length >= (MagickPathExtent-1))
                  token=(char *) AcquireQuantumMemory(length+MagickPathExtent,
                    sizeof(*token));
                if (token == (char *) NULL)
                  break;
                next=0;
                argument=argv[i+1];
                token_info=AcquireTokenInfo();
                token_status=Tokenizer(token_info,0,token,length,argument,"",
                  "=","\"",'\0',&breaker,&next,&quote);
                token_info=DestroyTokenInfo(token_info);
                if (token_status == 0)
                  {
                    const char
                      *arg;

                    arg=(&(argument[next]));
                    (void) InvokeDynamicImageFilter(token,&(*images),1,&arg,
                      exception);
                  }
                token=DestroyString(token);
                break;
              }
            (void) SubstituteString(&arguments[1],"-","");
            (void) InvokeDynamicImageFilter(arguments[1],&(*images),
              number_arguments-2,(const char **) arguments+2,exception);
            for (j=0; j < number_arguments; j++)
              arguments[j]=DestroyString(arguments[j]);
            arguments=(char **) RelinquishMagickMemory(arguments);
            break;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("reverse",option+1) == 0)
          {
            ReverseImageList(images);
            break;
          }
        break;
      }
      case 's':
      {
        if (LocaleCompare("smush",option+1) == 0)
          {
            Image
              *smush_image;

            ssize_t
              offset;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            offset=(ssize_t) StringToLong(argv[i+1]);
            smush_image=SmushImages(*images,*option == '-' ? MagickTrue :
              MagickFalse,offset,exception);
            if (smush_image == (Image *) NULL)
              {
                status=MagickFalse;
                break;
              }
            *images=DestroyImageList(*images);
            *images=smush_image;
            break;
          }
        if (LocaleCompare("swap",option+1) == 0)
          {
            Image
              *p,
              *q,
              *u,
              *v;

            ssize_t
              swap_index;

            index=(-1);
            swap_index=(-2);
            if (*option != '+')
              {
                GeometryInfo
                  geometry_info;

                MagickStatusType
                  flags;

                swap_index=(-1);
                flags=ParseGeometry(argv[i+1],&geometry_info);
                index=(ssize_t) geometry_info.rho;
                if ((flags & SigmaValue) != 0)
                  swap_index=(ssize_t) geometry_info.sigma;
              }
            p=GetImageFromList(*images,index);
            q=GetImageFromList(*images,swap_index);
            if ((p == (Image *) NULL) || (q == (Image *) NULL))
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"NoSuchImage","`%s'",(*images)->filename);
                status=MagickFalse;
                break;
              }
            if (p == q)
              break;
            u=CloneImage(p,0,0,MagickTrue,exception);
            if (u == (Image *) NULL)
              break;
            v=CloneImage(q,0,0,MagickTrue,exception);
            if (v == (Image *) NULL)
              {
                u=DestroyImage(u);
                break;
              }
            ReplaceImageInList(&p,v);
            ReplaceImageInList(&q,u);
            *images=GetFirstImageInList(q);
            break;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("write",option+1) == 0)
          {
            char
              key[MagickPathExtent];

            Image
              *write_images;

            ImageInfo
              *write_info;

            (void) SyncImagesSettings(mogrify_info,*images,exception);
            (void) FormatLocaleString(key,MagickPathExtent,"cache:%s",
              argv[i+1]);
            (void) DeleteImageRegistry(key);
            write_images=(*images);
            if (*option == '+')
              write_images=CloneImageList(*images,exception);
            write_info=CloneImageInfo(mogrify_info);
            status&=WriteImages(write_info,write_images,argv[i+1],exception);
            write_info=DestroyImageInfo(write_info);
            if (*option == '+')
              write_images=DestroyImageList(write_images);
            break;
          }
        break;
      }
      default:
        break;
    }
    i+=count;
  }
  quantize_info=DestroyQuantizeInfo(quantize_info);
  mogrify_info=DestroyImageInfo(mogrify_info);
  status&=MogrifyImageInfo(image_info,argc,argv,exception);
  return(status != 0 ? MagickTrue : MagickFalse);
}