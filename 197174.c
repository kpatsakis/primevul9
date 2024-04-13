WandExport MagickBooleanType MogrifyImage(ImageInfo *image_info,const int argc,
  const char **argv,Image **image,ExceptionInfo *exception)
{
  CompositeOperator
    compose;

  const char
    *format,
    *option;

  double
    attenuate;

  DrawInfo
    *draw_info;

  GeometryInfo
    geometry_info;

  ImageInfo
    *mogrify_info;

  MagickStatusType
    status;

  PixelInfo
    fill;

  MagickStatusType
    flags;

  PixelInterpolateMethod
    interpolate_method;

  QuantizeInfo
    *quantize_info;

  RectangleInfo
    geometry,
    region_geometry;

  register ssize_t
    i;

  /*
    Initialize method variables.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickCoreSignature);
  if ((*image)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",(*image)->filename);
  if (argc < 0)
    return(MagickTrue);
  mogrify_info=CloneImageInfo(image_info);
  draw_info=CloneDrawInfo(mogrify_info,(DrawInfo *) NULL);
  quantize_info=AcquireQuantizeInfo(mogrify_info);
  SetGeometryInfo(&geometry_info);
  GetPixelInfo(*image,&fill);
  fill=(*image)->background_color;
  attenuate=1.0;
  compose=(*image)->compose;
  interpolate_method=UndefinedInterpolatePixel;
  format=GetImageOption(mogrify_info,"format");
  SetGeometry(*image,&region_geometry);
  /*
    Transmogrify the image.
  */
  for (i=0; i < (ssize_t) argc; i++)
  {
    Image
      *mogrify_image;

    ssize_t
      count;

    option=argv[i];
    if (IsCommandOption(option) == MagickFalse)
      continue;
    count=MagickMax(ParseCommandOption(MagickCommandOptions,MagickFalse,option),
      0L);
    if ((i+count) >= (ssize_t) argc)
      break;
    status=MogrifyImageInfo(mogrify_info,(int) count+1,argv+i,exception);
    mogrify_image=(Image *) NULL;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adaptive-blur",option+1) == 0)
          {
            /*
              Adaptive blur image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=AdaptiveBlurImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("adaptive-resize",option+1) == 0)
          {
            /*
              Adaptive resize image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=AdaptiveResizeImage(*image,geometry.width,
              geometry.height,exception);
            break;
          }
        if (LocaleCompare("adaptive-sharpen",option+1) == 0)
          {
            /*
              Adaptive sharpen image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=AdaptiveSharpenImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("affine",option+1) == 0)
          {
            /*
              Affine matrix.
            */
            if (*option == '+')
              {
                GetAffineMatrix(&draw_info->affine);
                break;
              }
            (void) ParseAffineGeometry(argv[i+1],&draw_info->affine,exception);
            break;
          }
        if (LocaleCompare("alpha",option+1) == 0)
          {
            AlphaChannelOption
              alpha_type;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            alpha_type=(AlphaChannelOption) ParseCommandOption(
              MagickAlphaChannelOptions,MagickFalse,argv[i+1]);
            (void) SetImageAlphaChannel(*image,alpha_type,exception);
            break;
          }
        if (LocaleCompare("annotate",option+1) == 0)
          {
            char
              *text,
              geometry_str[MagickPathExtent];

            /*
              Annotate image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            SetGeometryInfo(&geometry_info);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            text=InterpretImageProperties(mogrify_info,*image,argv[i+2],
              exception);
            if (text == (char *) NULL)
              break;
            (void) CloneString(&draw_info->text,text);
            text=DestroyString(text);
            (void) FormatLocaleString(geometry_str,MagickPathExtent,"%+f%+f",
              geometry_info.xi,geometry_info.psi);
            (void) CloneString(&draw_info->geometry,geometry_str);
            draw_info->affine.sx=cos(DegreesToRadians(
              fmod(geometry_info.rho,360.0)));
            draw_info->affine.rx=sin(DegreesToRadians(
              fmod(geometry_info.rho,360.0)));
            draw_info->affine.ry=(-sin(DegreesToRadians(
              fmod(geometry_info.sigma,360.0))));
            draw_info->affine.sy=cos(DegreesToRadians(
              fmod(geometry_info.sigma,360.0)));
            (void) AnnotateImage(*image,draw_info,exception);
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            draw_info->stroke_antialias=(*option == '-') ? MagickTrue :
              MagickFalse;
            draw_info->text_antialias=(*option == '-') ? MagickTrue :
              MagickFalse;
            break;
          }
        if (LocaleCompare("attenuate",option+1) == 0)
          {
            if (*option == '+')
              {
                attenuate=1.0;
                break;
              }
            attenuate=StringToDouble(argv[i+1],(char **) NULL);
            break;
          }
        if (LocaleCompare("auto-gamma",option+1) == 0)
          {
            /*
              Auto Adjust Gamma of image based on its mean
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) AutoGammaImage(*image,exception);
            break;
          }
        if (LocaleCompare("auto-level",option+1) == 0)
          {
            /*
              Perfectly Normalize (max/min stretch) the image
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) AutoLevelImage(*image,exception);
            break;
          }
        if (LocaleCompare("auto-orient",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=AutoOrientImage(*image,(*image)->orientation,
              exception);
            break;
          }
        if (LocaleCompare("auto-threshold",option+1) == 0)
          {
            AutoThresholdMethod
              method;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            method=(AutoThresholdMethod) ParseCommandOption(
              MagickAutoThresholdOptions,MagickFalse,argv[i+1]);
            (void) AutoThresholdImage(*image,method,exception);
            break;
          }
        break;
      }
      case 'b':
      {
        if (LocaleCompare("black-threshold",option+1) == 0)
          {
            /*
              Black threshold image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) BlackThresholdImage(*image,argv[i+1],exception);
            break;
          }
        if (LocaleCompare("blue-shift",option+1) == 0)
          {
            /*
              Blue shift image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            geometry_info.rho=1.5;
            if (*option == '-')
              flags=ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=BlueShiftImage(*image,geometry_info.rho,exception);
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            /*
              Gaussian blur image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=0.0;
            mogrify_image=BlurImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            /*
              Surround image with a border of solid color.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParsePageGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=BorderImage(*image,&geometry,compose,exception);
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) QueryColorCompliance(MogrifyBorderColor,AllCompliance,
                  &draw_info->border_color,exception);
                break;
              }
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &draw_info->border_color,exception);
            break;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &draw_info->undercolor,exception);
            break;
          }
        if (LocaleCompare("brightness-contrast",option+1) == 0)
          {
            double
              brightness,
              contrast;

            /*
              Brightness / contrast image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            brightness=geometry_info.rho;
            contrast=0.0;
            if ((flags & SigmaValue) != 0)
              contrast=geometry_info.sigma;
            (void) BrightnessContrastImage(*image,brightness,contrast,
              exception);
            break;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("canny",option+1) == 0)
          {
            /*
              Detect edges in the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=0.10;
            if ((flags & PsiValue) == 0)
              geometry_info.psi=0.30;
            if ((flags & PercentValue) != 0)
              {
                geometry_info.xi/=100.0;
                geometry_info.psi/=100.0;
              }
            mogrify_image=CannyEdgeImage(*image,geometry_info.rho,
              geometry_info.sigma,geometry_info.xi,geometry_info.psi,exception);
            break;
          }
        if (LocaleCompare("cdl",option+1) == 0)
          {
            char
              *color_correction_collection;

            /*
              Color correct with a color decision list.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            color_correction_collection=FileToString(argv[i+1],~0UL,exception);
            if (color_correction_collection == (char *) NULL)
              break;
            (void) ColorDecisionListImage(*image,color_correction_collection,
              exception);
            break;
          }
        if (LocaleCompare("channel",option+1) == 0)
          {
            ChannelType
              channel;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                (void) SetPixelChannelMask(*image,DefaultChannels);
                break;
              }
            channel=(ChannelType) ParseChannelOption(argv[i+1]);
            (void) SetPixelChannelMask(*image,channel);
            break;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            /*
              Charcoal image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=1.0;
            mogrify_image=CharcoalImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            /*
              Chop the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseGravityGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=ChopImage(*image,&geometry,exception);
            break;
          }
        if (LocaleCompare("clahe",option+1) == 0)
          {
            /*
              Contrast limited adaptive histogram equalization.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            (void) CLAHEImage(*image,geometry.width,geometry.height,
              (size_t) geometry.x,geometry_info.psi,exception);
            break;
          }
        if (LocaleCompare("clip",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                (void) SetImageMask(*image,WritePixelMask,(Image *) NULL,
                  exception);
                break;
              }
            (void) ClipImage(*image,exception);
            break;
          }
        if (LocaleCompare("clip-mask",option+1) == 0)
          {
            Image
              *clip_mask;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                /*
                  Remove a mask.
                */
                (void) SetImageMask(*image,WritePixelMask,(Image *) NULL,
                  exception);
                break;
              }
            /*
              Set the image mask.
            */
            clip_mask=GetImageCache(mogrify_info,argv[i+1],exception);
            if (clip_mask == (Image *) NULL)
              break;
            (void) SetImageMask(*image,WritePixelMask,clip_mask,exception);
            clip_mask=DestroyImage(clip_mask);
            break;
          }
        if (LocaleCompare("clip-path",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ClipImagePath(*image,argv[i+1],*option == '-' ? MagickTrue :
              MagickFalse,exception);
            break;
          }
        if (LocaleCompare("colorize",option+1) == 0)
          {
            /*
              Colorize the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=ColorizeImage(*image,argv[i+1],&fill,exception);
            break;
          }
        if (LocaleCompare("color-matrix",option+1) == 0)
          {
            KernelInfo
              *kernel;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            kernel=AcquireKernelInfo(argv[i+1],exception);
            if (kernel == (KernelInfo *) NULL)
              break;
            /* FUTURE: check on size of the matrix */
            mogrify_image=ColorMatrixImage(*image,kernel,exception);
            kernel=DestroyKernelInfo(kernel);
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            /*
              Reduce the number of colors in the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            quantize_info->number_colors=StringToUnsignedLong(argv[i+1]);
            if (quantize_info->number_colors == 0)
              break;
            if (((*image)->storage_class == DirectClass) ||
                (*image)->colors > quantize_info->number_colors)
              (void) QuantizeImage(quantize_info,*image,exception);
            else
              (void) CompressImageColormap(*image,exception);
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            ColorspaceType
              colorspace;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                (void) TransformImageColorspace(*image,sRGBColorspace,
                  exception);
                break;
              }
            colorspace=(ColorspaceType) ParseCommandOption(
              MagickColorspaceOptions,MagickFalse,argv[i+1]);
            (void) TransformImageColorspace(*image,colorspace,exception);
            break;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            compose=(CompositeOperator) ParseCommandOption(MagickComposeOptions,
              MagickFalse,argv[i+1]);
            break;
          }
        if (LocaleCompare("connected-components",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=ConnectedComponentsImage(*image,(size_t)
              StringToInteger(argv[i+1]),(CCObjectInfo **) NULL,exception);
            break;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ContrastImage(*image,(*option == '-') ? MagickTrue :
              MagickFalse,exception);
            break;
          }
        if (LocaleCompare("contrast-stretch",option+1) == 0)
          {
            double
              black_point,
              white_point;

            /*
              Contrast stretch image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            black_point=geometry_info.rho;
            white_point=(flags & SigmaValue) != 0 ? geometry_info.sigma :
              black_point;
            if ((flags & PercentValue) != 0)
              {
                black_point*=(double) (*image)->columns*(*image)->rows/100.0;
                white_point*=(double) (*image)->columns*(*image)->rows/100.0;
              }
            white_point=(double) (*image)->columns*(*image)->rows-
              white_point;
            (void) ContrastStretchImage(*image,black_point,white_point,
              exception);
            break;
          }
        if (LocaleCompare("convolve",option+1) == 0)
          {
            double
              gamma;

            KernelInfo
              *kernel_info;

            register ssize_t
              j;

            size_t
              extent;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            kernel_info=AcquireKernelInfo(argv[i+1],exception);
            if (kernel_info == (KernelInfo *) NULL)
              break;
            extent=kernel_info->width*kernel_info->height;
            gamma=0.0;
            for (j=0; j < (ssize_t) extent; j++)
              gamma+=kernel_info->values[j];
            gamma=1.0/(fabs((double) gamma) <= MagickEpsilon ? 1.0 : gamma);
            for (j=0; j < (ssize_t) extent; j++)
              kernel_info->values[j]*=gamma;
            mogrify_image=MorphologyImage(*image,CorrelateMorphology,1,
              kernel_info,exception);
            kernel_info=DestroyKernelInfo(kernel_info);
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            /*
              Crop a image to a smaller size
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=CropImageToTiles(*image,argv[i+1],exception);
            break;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            /*
              Cycle an image colormap.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) CycleColormapImage(*image,(ssize_t) StringToLong(argv[i+1]),
              exception);
            break;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("decipher",option+1) == 0)
          {
            StringInfo
              *passkey;

            /*
              Decipher pixels.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            passkey=FileToStringInfo(argv[i+1],~0UL,exception);
            if (passkey != (StringInfo *) NULL)
              {
                (void) PasskeyDecipherImage(*image,passkey,exception);
                passkey=DestroyStringInfo(passkey);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            /*
              Set image density.
            */
            (void) CloneString(&draw_info->density,argv[i+1]);
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                (void) SetImageDepth(*image,MAGICKCORE_QUANTUM_DEPTH,exception);
                break;
              }
            (void) SetImageDepth(*image,StringToUnsignedLong(argv[i+1]),
              exception);
            break;
          }
        if (LocaleCompare("deskew",option+1) == 0)
          {
            double
              threshold;

            /*
              Straighten the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              threshold=40.0*QuantumRange/100.0;
            else
              threshold=StringToDoubleInterval(argv[i+1],(double) QuantumRange+
                1.0);
            mogrify_image=DeskewImage(*image,threshold,exception);
            break;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          {
            /*
              Reduce the speckles within an image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=DespeckleImage(*image,exception);
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&draw_info->server_name,argv[i+1]);
            break;
          }
        if (LocaleCompare("distort",option+1) == 0)
          {
            char
              *args,
              token[MagickPathExtent];

            const char
              *p;

            DistortMethod
              method;

            double
              *arguments;

            register ssize_t
              x;

            size_t
              number_arguments;

            /*
              Distort image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            method=(DistortMethod) ParseCommandOption(MagickDistortOptions,
              MagickFalse,argv[i+1]);
            if (method == ResizeDistortion)
              {
                 double
                   resize_args[2];

                 /*
                   Special Case - Argument is actually a resize geometry!
                   Convert that to an appropriate distortion argument array.
                 */
                 (void) ParseRegionGeometry(*image,argv[i+2],&geometry,
                   exception);
                 resize_args[0]=(double) geometry.width;
                 resize_args[1]=(double) geometry.height;
                 mogrify_image=DistortImage(*image,method,(size_t)2,
                   resize_args,MagickTrue,exception);
                 break;
              }
            args=InterpretImageProperties(mogrify_info,*image,argv[i+2],
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
                "MemoryAllocationFailed",(*image)->filename);
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
            mogrify_image=DistortImage(*image,method,number_arguments,arguments,
              (*option == '+') ? MagickTrue : MagickFalse,exception);
            arguments=(double *) RelinquishMagickMemory(arguments);
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
        if (LocaleCompare("draw",option+1) == 0)
          {
            /*
              Draw image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) CloneString(&draw_info->primitive,argv[i+1]);
            (void) DrawImage(*image,draw_info,exception);
            break;
          }
        break;
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            /*
              Enhance edges in the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=EdgeImage(*image,geometry_info.rho,exception);
            break;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            /*
              Emboss image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=EmbossImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("encipher",option+1) == 0)
          {
            StringInfo
              *passkey;

            /*
              Encipher pixels.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            passkey=FileToStringInfo(argv[i+1],~0UL,exception);
            if (passkey != (StringInfo *) NULL)
              {
                (void) PasskeyEncipherImage(*image,passkey,exception);
                passkey=DestroyStringInfo(passkey);
              }
            break;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            (void) CloneString(&draw_info->encoding,argv[i+1]);
            break;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          {
            /*
              Enhance image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=EnhanceImage(*image,exception);
            break;
          }
        if (LocaleCompare("equalize",option+1) == 0)
          {
            /*
              Equalize image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) EqualizeImage(*image,exception);
            break;
          }
        if (LocaleCompare("evaluate",option+1) == 0)
          {
            double
              constant;

            MagickEvaluateOperator
              op;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            op=(MagickEvaluateOperator) ParseCommandOption(
              MagickEvaluateOptions,MagickFalse,argv[i+1]);
            constant=StringToDoubleInterval(argv[i+2],(double) QuantumRange+
              1.0);
            (void) EvaluateImage(*image,op,constant,exception);
            break;
          }
        if (LocaleCompare("extent",option+1) == 0)
          {
            /*
              Set the image extent.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGravityGeometry(*image,argv[i+1],&geometry,exception);
            if (geometry.width == 0)
              geometry.width=(*image)->columns;
            if (geometry.height == 0)
              geometry.height=(*image)->rows;
            mogrify_image=ExtentImage(*image,&geometry,exception);
            break;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("family",option+1) == 0)
          {
            if (*option == '+')
              {
                if (draw_info->family != (char *) NULL)
                  draw_info->family=DestroyString(draw_info->family);
                break;
              }
            (void) CloneString(&draw_info->family,argv[i+1]);
            break;
          }
        if (LocaleCompare("features",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageArtifact(*image,"identify:features");
                break;
              }
            (void) SetImageArtifact(*image,"identify:features",argv[i+1]);
            (void) SetImageArtifact(*image,"verbose","true");
            break;
          }
        if (LocaleCompare("fill",option+1) == 0)
          {
            ExceptionInfo
              *sans;

            PixelInfo
              color;

            GetPixelInfo(*image,&fill);
            if (*option == '+')
              {
                (void) QueryColorCompliance("none",AllCompliance,&fill,
                  exception);
                draw_info->fill=fill;
                if (draw_info->fill_pattern != (Image *) NULL)
                  draw_info->fill_pattern=DestroyImage(draw_info->fill_pattern);
                break;
              }
            sans=AcquireExceptionInfo();
            status=QueryColorCompliance(argv[i+1],AllCompliance,&color,sans);
            sans=DestroyExceptionInfo(sans);
            if (status == MagickFalse)
              draw_info->fill_pattern=GetImageCache(mogrify_info,argv[i+1],
                exception);
            else
              draw_info->fill=fill=color;
            break;
          }
        if (LocaleCompare("flip",option+1) == 0)
          {
            /*
              Flip image scanlines.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=FlipImage(*image,exception);
            break;
          }
        if (LocaleCompare("floodfill",option+1) == 0)
          {
            PixelInfo
              target;

            /*
              Floodfill image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParsePageGeometry(*image,argv[i+1],&geometry,exception);
            (void) QueryColorCompliance(argv[i+2],AllCompliance,&target,
              exception);
            (void) FloodfillPaintImage(*image,draw_info,&target,geometry.x,
              geometry.y,*option == '-' ? MagickFalse : MagickTrue,exception);
            break;
          }
        if (LocaleCompare("flop",option+1) == 0)
          {
            /*
              Flop image scanlines.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=FlopImage(*image,exception);
            break;
          }
        if (LocaleCompare("font",option+1) == 0)
          {
            if (*option == '+')
              {
                if (draw_info->font != (char *) NULL)
                  draw_info->font=DestroyString(draw_info->font);
                break;
              }
            (void) CloneString(&draw_info->font,argv[i+1]);
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            format=argv[i+1];
            break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            FrameInfo
              frame_info;

            /*
              Surround image with an ornamental border.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParsePageGeometry(*image,argv[i+1],&geometry,exception);
            frame_info.width=geometry.width;
            frame_info.height=geometry.height;
            frame_info.outer_bevel=geometry.x;
            frame_info.inner_bevel=geometry.y;
            frame_info.x=(ssize_t) frame_info.width;
            frame_info.y=(ssize_t) frame_info.height;
            frame_info.width=(*image)->columns+2*frame_info.width;
            frame_info.height=(*image)->rows+2*frame_info.height;
            mogrify_image=FrameImage(*image,&frame_info,compose,exception);
            break;
          }
        if (LocaleCompare("function",option+1) == 0)
          {
            char
              *arguments,
              token[MagickPathExtent];

            const char
              *p;

            double
              *parameters;

            MagickFunction
              function;

            register ssize_t
              x;

            size_t
              number_parameters;

            /*
              Function Modify Image Values
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            function=(MagickFunction) ParseCommandOption(MagickFunctionOptions,
              MagickFalse,argv[i+1]);
            arguments=InterpretImageProperties(mogrify_info,*image,argv[i+2],
              exception);
            if (arguments == (char *) NULL)
              break;
            p=(char *) arguments;
            for (x=0; *p != '\0'; x++)
            {
              GetNextToken(p,&p,MagickPathExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MagickPathExtent,token);
            }
            number_parameters=(size_t) x;
            parameters=(double *) AcquireQuantumMemory(number_parameters,
              sizeof(*parameters));
            if (parameters == (double *) NULL)
              ThrowWandFatalException(ResourceLimitFatalError,
                "MemoryAllocationFailed",(*image)->filename);
            (void) memset(parameters,0,number_parameters*
              sizeof(*parameters));
            p=(char *) arguments;
            for (x=0; (x < (ssize_t) number_parameters) && (*p != '\0'); x++)
            {
              GetNextToken(p,&p,MagickPathExtent,token);
              if (*token == ',')
                GetNextToken(p,&p,MagickPathExtent,token);
              parameters[x]=StringToDouble(token,(char **) NULL);
            }
            arguments=DestroyString(arguments);
            (void) FunctionImage(*image,function,number_parameters,parameters,
              exception);
            parameters=(double *) RelinquishMagickMemory(parameters);
            break;
          }
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            /*
              Gamma image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              (*image)->gamma=StringToDouble(argv[i+1],(char **) NULL);
            else
              (void) GammaImage(*image,StringToDouble(argv[i+1],(char **) NULL),
                exception);
            break;
          }
        if ((LocaleCompare("gaussian-blur",option+1) == 0) ||
            (LocaleCompare("gaussian",option+1) == 0))
          {
            /*
              Gaussian blur image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=GaussianBlurImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
              /*
                Record Image offset, Resize last image.
              */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                if ((*image)->geometry != (char *) NULL)
                  (*image)->geometry=DestroyString((*image)->geometry);
                break;
              }
            flags=ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            if (((flags & XValue) != 0) || ((flags & YValue) != 0))
              (void) CloneString(&(*image)->geometry,argv[i+1]);
            else
              mogrify_image=ResizeImage(*image,geometry.width,geometry.height,
                (*image)->filter,exception);
            break;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            if (*option == '+')
              {
                draw_info->gravity=UndefinedGravity;
                break;
              }
            draw_info->gravity=(GravityType) ParseCommandOption(
              MagickGravityOptions,MagickFalse,argv[i+1]);
            break;
          }
        if (LocaleCompare("grayscale",option+1) == 0)
          {
            PixelIntensityMethod
              method;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            method=(PixelIntensityMethod) ParseCommandOption(
              MagickPixelIntensityOptions,MagickFalse,argv[i+1]);
            (void) GrayscaleImage(*image,method,exception);
            break;
          }
        break;
      }
      case 'h':
      {
        if (LocaleCompare("highlight-color",option+1) == 0)
          {
            (void) SetImageArtifact(*image,"compare:highlight-color",argv[i+1]);
            break;
          }
        if (LocaleCompare("hough-lines",option+1) == 0)
          {
            /*
              Detect edges in the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            if ((flags & XiValue) == 0)
              geometry_info.xi=40;
            mogrify_image=HoughLineImage(*image,(size_t) geometry_info.rho,
              (size_t) geometry_info.sigma,(size_t) geometry_info.xi,exception);
            break;
          }
        break;
      }
      case 'i':
      {
        if (LocaleCompare("identify",option+1) == 0)
          {
            char
              *text;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (format == (char *) NULL)
              {
                (void) IdentifyImage(*image,stdout,mogrify_info->verbose,
                  exception);
                break;
              }
            text=InterpretImageProperties(mogrify_info,*image,format,
              exception);
            if (text == (char *) NULL)
              break;
            (void) fputs(text,stdout);
            text=DestroyString(text);
            break;
          }
        if (LocaleCompare("implode",option+1) == 0)
          {
            /*
              Implode image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=ImplodeImage(*image,geometry_info.rho,
              interpolate_method,exception);
            break;
          }
        if (LocaleCompare("interline-spacing",option+1) == 0)
          {
            if (*option == '+')
              (void) ParseGeometry("0",&geometry_info);
            else
              (void) ParseGeometry(argv[i+1],&geometry_info);
            draw_info->interline_spacing=geometry_info.rho;
            break;
          }
        if (LocaleCompare("interpolate",option+1) == 0)
          {
            interpolate_method=(PixelInterpolateMethod) ParseCommandOption(
              MagickInterpolateOptions,MagickFalse,argv[i+1]);
            break;
          }
        if (LocaleCompare("interword-spacing",option+1) == 0)
          {
            if (*option == '+')
              (void) ParseGeometry("0",&geometry_info);
            else
              (void) ParseGeometry(argv[i+1],&geometry_info);
            draw_info->interword_spacing=geometry_info.rho;
            break;
          }
        if (LocaleCompare("interpolative-resize",option+1) == 0)
          {
            /*
              Interpolative resize image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=InterpolativeResizeImage(*image,geometry.width,
              geometry.height,interpolate_method,exception);
            break;
          }
        break;
      }
      case 'k':
      {
        if (LocaleCompare("kerning",option+1) == 0)
          {
            if (*option == '+')
              (void) ParseGeometry("0",&geometry_info);
            else
              (void) ParseGeometry(argv[i+1],&geometry_info);
            draw_info->kerning=geometry_info.rho;
            break;
          }
        if (LocaleCompare("kuwahara",option+1) == 0)
          {
            /*
              Edge preserving blur.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho-0.5;
            mogrify_image=KuwaharaImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        break;
      }
      case 'l':
      {
        if (LocaleCompare("lat",option+1) == 0)
          {
            /*
              Local adaptive threshold image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & PercentValue) != 0)
              geometry_info.xi=(double) QuantumRange*geometry_info.xi/100.0;
            mogrify_image=AdaptiveThresholdImage(*image,(size_t)
              geometry_info.rho,(size_t) geometry_info.sigma,(double)
              geometry_info.xi,exception);
            break;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            double
              black_point,
              gamma,
              white_point;

            /*
              Parse levels.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            black_point=geometry_info.rho;
            white_point=(double) QuantumRange;
            if ((flags & SigmaValue) != 0)
              white_point=geometry_info.sigma;
            gamma=1.0;
            if ((flags & XiValue) != 0)
              gamma=geometry_info.xi;
            if ((flags & PercentValue) != 0)
              {
                black_point*=(double) (QuantumRange/100.0);
                white_point*=(double) (QuantumRange/100.0);
              }
            if ((flags & SigmaValue) == 0)
              white_point=(double) QuantumRange-black_point;
            if ((*option == '+') || ((flags & AspectValue) != 0))
              (void) LevelizeImage(*image,black_point,white_point,gamma,
                exception);
            else
              (void) LevelImage(*image,black_point,white_point,gamma,
                exception);
            break;
          }
        if (LocaleCompare("level-colors",option+1) == 0)
          {
            char
              token[MagickPathExtent];

            const char
              *p;

            PixelInfo
              black_point,
              white_point;

            p=(const char *) argv[i+1];
            GetNextToken(p,&p,MagickPathExtent,token);  /* get black point color */
            if ((isalpha((int) *token) != 0) || ((*token == '#') != 0))
              (void) QueryColorCompliance(token,AllCompliance,
                &black_point,exception);
            else
              (void) QueryColorCompliance("#000000",AllCompliance,
                &black_point,exception);
            if (isalpha((int) token[0]) || (token[0] == '#'))
              GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == '\0')
              white_point=black_point; /* set everything to that color */
            else
              {
                if ((isalpha((int) *token) == 0) && ((*token == '#') == 0))
                  GetNextToken(p,&p,MagickPathExtent,token); /* Get white point color. */
                if ((isalpha((int) *token) != 0) || ((*token == '#') != 0))
                  (void) QueryColorCompliance(token,AllCompliance,
                    &white_point,exception);
                else
                  (void) QueryColorCompliance("#ffffff",AllCompliance,
                    &white_point,exception);
              }
            (void) LevelImageColors(*image,&black_point,&white_point,
              *option == '+' ? MagickTrue : MagickFalse,exception);
            break;
          }
        if (LocaleCompare("linear-stretch",option+1) == 0)
          {
            double
              black_point,
              white_point;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            black_point=geometry_info.rho;
            white_point=(double) (*image)->columns*(*image)->rows;
            if ((flags & SigmaValue) != 0)
              white_point=geometry_info.sigma;
            if ((flags & PercentValue) != 0)
              {
                black_point*=(double) (*image)->columns*(*image)->rows/100.0;
                white_point*=(double) (*image)->columns*(*image)->rows/100.0;
              }
            if ((flags & SigmaValue) == 0)
              white_point=(double) (*image)->columns*(*image)->rows-
                black_point;
            (void) LinearStretchImage(*image,black_point,white_point,exception);
            break;
          }
        if (LocaleCompare("liquid-rescale",option+1) == 0)
          {
            /*
              Liquid rescale image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            if ((flags & XValue) == 0)
              geometry.x=1;
            if ((flags & YValue) == 0)
              geometry.y=0;
            mogrify_image=LiquidRescaleImage(*image,geometry.width,
              geometry.height,1.0*geometry.x,1.0*geometry.y,exception);
            break;
          }
        if (LocaleCompare("local-contrast",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & RhoValue) == 0)
              geometry_info.rho=10;
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=12.5;
            mogrify_image=LocalContrastImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("lowlight-color",option+1) == 0)
          {
            (void) SetImageArtifact(*image,"compare:lowlight-color",argv[i+1]);
            break;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("magnify",option+1) == 0)
          {
            /*
              Double image size.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=MagnifyImage(*image,exception);
            break;
          }
        if (LocaleCompare("map",option+1) == 0)
          {
            Image
              *remap_image;

            /*
              Transform image colors to match this set of colors.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              break;
            remap_image=GetImageCache(mogrify_info,argv[i+1],exception);
            if (remap_image == (Image *) NULL)
              break;
            (void) RemapImage(quantize_info,*image,remap_image,exception);
            remap_image=DestroyImage(remap_image);
            break;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            Image
              *mask;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                /*
                  Remove a mask.
                */
                (void) SetImageMask(*image,WritePixelMask,(Image *) NULL,
                  exception);
                break;
              }
            /*
              Set the image mask.
            */
            mask=GetImageCache(mogrify_info,argv[i+1],exception);
            if (mask == (Image *) NULL)
              break;
            (void) SetImageMask(*image,WritePixelMask,mask,exception);
            mask=DestroyImage(mask);
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          {
            (void) SetImageAlphaChannel(*image,(*option == '-') ?
              SetAlphaChannel : DeactivateAlphaChannel,exception);
            break;
          }
        if (LocaleCompare("mean-shift",option+1) == 0)
          {
            /*
              Detect edges in the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            if ((flags & XiValue) == 0)
              geometry_info.xi=0.10*QuantumRange;
            if ((flags & PercentValue) != 0)
              geometry_info.xi=(double) QuantumRange*geometry_info.xi/100.0;
            mogrify_image=MeanShiftImage(*image,(size_t) geometry_info.rho,
              (size_t) geometry_info.sigma,geometry_info.xi,exception);
            break;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            /*
              Median filter image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            mogrify_image=StatisticImage(*image,MedianStatistic,(size_t)
              geometry_info.rho,(size_t) geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("mode",option+1) == 0)
          {
            /*
              Mode image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            mogrify_image=StatisticImage(*image,ModeStatistic,(size_t)
              geometry_info.rho,(size_t) geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("modulate",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ModulateImage(*image,argv[i+1],exception);
            break;
          }
        if (LocaleCompare("moments",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageArtifact(*image,"identify:moments");
                break;
              }
            (void) SetImageArtifact(*image,"identify:moments",argv[i+1]);
            (void) SetImageArtifact(*image,"verbose","true");
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageProgressMonitor(*image,
                  (MagickProgressMonitor) NULL,(void *) NULL);
                break;
              }
            (void) SetImageProgressMonitor(*image,MonitorProgress,
              (void *) NULL);
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) SetImageType(*image,BilevelType,exception);
            break;
          }
        if (LocaleCompare("morphology",option+1) == 0)
          {
            char
              token[MagickPathExtent];

            const char
              *p;

            KernelInfo
              *kernel;

            MorphologyMethod
              method;

            ssize_t
              iterations;

            /*
              Morphological Image Operation
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            p=argv[i+1];
            GetNextToken(p,&p,MagickPathExtent,token);
            method=(MorphologyMethod) ParseCommandOption(
              MagickMorphologyOptions,MagickFalse,token);
            iterations=1L;
            GetNextToken(p,&p,MagickPathExtent,token);
            if ((*p == ':') || (*p == ','))
              GetNextToken(p,&p,MagickPathExtent,token);
            if ((*p != '\0'))
              iterations=(ssize_t) StringToLong(p);
            kernel=AcquireKernelInfo(argv[i+2],exception);
            if (kernel == (KernelInfo *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"UnabletoParseKernel","morphology");
                status=MagickFalse;
                break;
              }
            mogrify_image=MorphologyImage(*image,method,iterations,kernel,
              exception);
            kernel=DestroyKernelInfo(kernel);
            break;
          }
        if (LocaleCompare("motion-blur",option+1) == 0)
          {
            /*
              Motion blur image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=MotionBlurImage(*image,geometry_info.rho,
              geometry_info.sigma,geometry_info.xi,exception);
            break;
          }
        break;
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) NegateImage(*image,*option == '+' ? MagickTrue :
              MagickFalse,exception);
            break;
          }
        if (LocaleCompare("noise",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '-')
              {
                flags=ParseGeometry(argv[i+1],&geometry_info);
                if ((flags & SigmaValue) == 0)
                  geometry_info.sigma=geometry_info.rho;
                mogrify_image=StatisticImage(*image,NonpeakStatistic,(size_t)
                  geometry_info.rho,(size_t) geometry_info.sigma,exception);
              }
            else
              {
                NoiseType
                  noise;

                noise=(NoiseType) ParseCommandOption(MagickNoiseOptions,
                  MagickFalse,argv[i+1]);
                mogrify_image=AddNoiseImage(*image,noise,attenuate,exception);
              }
            break;
          }
        if (LocaleCompare("normalize",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) NormalizeImage(*image,exception);
            break;
          }
        break;
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            PixelInfo
              target;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) QueryColorCompliance(argv[i+1],AllCompliance,&target,
              exception);
            (void) OpaquePaintImage(*image,&target,&fill,*option == '-' ?
              MagickFalse : MagickTrue,exception);
            break;
          }
        if (LocaleCompare("ordered-dither",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) OrderedDitherImage(*image,argv[i+1],exception);
            break;
          }
        break;
      }
      case 'p':
      {
        if (LocaleCompare("paint",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=OilPaintImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("perceptible",option+1) == 0)
          {
            /*
              Perceptible image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) PerceptibleImage(*image,StringToDouble(argv[i+1],
              (char **) NULL),exception);
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            if (*option == '+')
              (void) ParseGeometry("12",&geometry_info);
            else
              (void) ParseGeometry(argv[i+1],&geometry_info);
            draw_info->pointsize=geometry_info.rho;
            break;
          }
        if (LocaleCompare("polaroid",option+1) == 0)
          {
            const char
              *caption;

            double
              angle;

            RandomInfo
              *random_info;

            /*
              Simulate a Polaroid picture.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            random_info=AcquireRandomInfo();
            angle=22.5*(GetPseudoRandomValue(random_info)-0.5);
            random_info=DestroyRandomInfo(random_info);
            if (*option == '-')
              {
                SetGeometryInfo(&geometry_info);
                flags=ParseGeometry(argv[i+1],&geometry_info);
                angle=geometry_info.rho;
              }
            caption=GetImageProperty(*image,"caption",exception);
            mogrify_image=PolaroidImage(*image,draw_info,caption,angle,
              interpolate_method,exception);
            break;
          }
        if (LocaleCompare("posterize",option+1) == 0)
          {
            /*
              Posterize image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) PosterizeImage(*image,StringToUnsignedLong(argv[i+1]),
              quantize_info->dither_method,exception);
            break;
          }
        if (LocaleCompare("preview",option+1) == 0)
          {
            PreviewType
              preview_type;

            /*
              Preview image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              preview_type=UndefinedPreview;
            else
              preview_type=(PreviewType) ParseCommandOption(
                MagickPreviewOptions,MagickFalse,argv[i+1]);
            mogrify_image=PreviewImage(*image,preview_type,exception);
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            const char
              *name;

            const StringInfo
              *profile;

            Image
              *profile_image;

            ImageInfo
              *profile_info;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                /*
                  Remove a profile from the image.
                */
                (void) ProfileImage(*image,argv[i+1],(const unsigned char *)
                  NULL,0,exception);
                break;
              }
            /*
              Associate a profile with the image.
            */
            profile_info=CloneImageInfo(mogrify_info);
            profile=GetImageProfile(*image,"iptc");
            if (profile != (StringInfo *) NULL)
              profile_info->profile=(void *) CloneStringInfo(profile);
            profile_image=GetImageCache(profile_info,argv[i+1],exception);
            profile_info=DestroyImageInfo(profile_info);
            if (profile_image == (Image *) NULL)
              {
                StringInfo
                  *file_data;

                profile_info=CloneImageInfo(mogrify_info);
                (void) CopyMagickString(profile_info->filename,argv[i+1],
                  MagickPathExtent);
                file_data=FileToStringInfo(profile_info->filename,~0UL,
                  exception);
                if (file_data != (StringInfo *) NULL)
                  {
                    (void) SetImageInfo(profile_info,0,exception);
                    (void) ProfileImage(*image,profile_info->magick,
                      GetStringInfoDatum(file_data),
                      GetStringInfoLength(file_data),exception);
                    file_data=DestroyStringInfo(file_data);
                  }
                profile_info=DestroyImageInfo(profile_info);
                break;
              }
            ResetImageProfileIterator(profile_image);
            name=GetNextImageProfile(profile_image);
            while (name != (const char *) NULL)
            {
              profile=GetImageProfile(profile_image,name);
              if (profile != (StringInfo *) NULL)
                (void) ProfileImage(*image,name,GetStringInfoDatum(profile),
                  (size_t) GetStringInfoLength(profile),exception);
              name=GetNextImageProfile(profile_image);
            }
            profile_image=DestroyImage(profile_image);
            break;
          }
        break;
      }
      case 'q':
      {
        if (LocaleCompare("quantize",option+1) == 0)
          {
            if (*option == '+')
              {
                quantize_info->colorspace=UndefinedColorspace;
                break;
              }
            quantize_info->colorspace=(ColorspaceType) ParseCommandOption(
              MagickColorspaceOptions,MagickFalse,argv[i+1]);
            break;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("rotational-blur",option+1) == 0)
          {
            /*
              Rotational blur image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=RotationalBlurImage(*image,geometry_info.rho,
              exception);
            break;
          }
        if (LocaleCompare("raise",option+1) == 0)
          {
            /*
              Surround image with a raise of solid color.
            */
            flags=ParsePageGeometry(*image,argv[i+1],&geometry,exception);
            (void) RaiseImage(*image,&geometry,*option == '-' ? MagickTrue :
              MagickFalse,exception);
            break;
          }
        if (LocaleCompare("random-threshold",option+1) == 0)
          {
            /*
              Random threshold image.
            */
            double
              min_threshold,
              max_threshold;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            min_threshold=0.0;
            max_threshold=(double) QuantumRange;
            flags=ParseGeometry(argv[i+1],&geometry_info);
            min_threshold=geometry_info.rho;
            max_threshold=geometry_info.sigma;
            if ((flags & SigmaValue) == 0)
              max_threshold=min_threshold;
            if (strchr(argv[i+1],'%') != (char *) NULL)
              {
                max_threshold*=(double) (0.01*QuantumRange);
                min_threshold*=(double) (0.01*QuantumRange);
              }
            (void) RandomThresholdImage(*image,min_threshold,max_threshold,
              exception);
            break;
          }
        if (LocaleCompare("range-threshold",option+1) == 0)
          {
            /*
              Range threshold image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            if ((flags & XiValue) == 0)
              geometry_info.xi=geometry_info.sigma;
            if ((flags & PsiValue) == 0)
              geometry_info.psi=geometry_info.xi;
            if (strchr(argv[i+1],'%') != (char *) NULL)
              {
                geometry_info.rho*=(double) (0.01*QuantumRange);
                geometry_info.sigma*=(double) (0.01*QuantumRange);
                geometry_info.xi*=(double) (0.01*QuantumRange);
                geometry_info.psi*=(double) (0.01*QuantumRange);
              }
            (void) RangeThresholdImage(*image,geometry_info.rho,
              geometry_info.sigma,geometry_info.xi,geometry_info.psi,exception);
            break;
          }
        if (LocaleCompare("read-mask",option+1) == 0)
          {
            Image
              *mask;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                /*
                  Remove a mask.
                */
                (void) SetImageMask(*image,ReadPixelMask,(Image *) NULL,
                  exception);
                break;
              }
            /*
              Set the image mask.
            */
            mask=GetImageCache(mogrify_info,argv[i+1],exception);
            if (mask == (Image *) NULL)
              break;
            (void) SetImageMask(*image,ReadPixelMask,mask,exception);
            mask=DestroyImage(mask);
            break;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            /*
              Apply read mask as defined by a region geometry.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                (void) SetImageRegionMask(*image,WritePixelMask,
                  (const RectangleInfo *) NULL,exception);
                break;
              }
            (void) ParseGravityGeometry(*image,argv[i+1],&geometry,exception);
            (void) SetImageRegionMask(*image,WritePixelMask,&geometry,
              exception);
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          {
            (void) SyncImageSettings(mogrify_info,*image,exception);
            draw_info->render=(*option == '+') ? MagickTrue : MagickFalse;
            break;
          }
        if (LocaleCompare("remap",option+1) == 0)
          {
            Image
              *remap_image;

            /*
              Transform image colors to match this set of colors.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              break;
            remap_image=GetImageCache(mogrify_info,argv[i+1],exception);
            if (remap_image == (Image *) NULL)
              break;
            (void) RemapImage(quantize_info,*image,remap_image,exception);
            remap_image=DestroyImage(remap_image);
            break;
          }
        if (LocaleCompare("repage",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) ParseAbsoluteGeometry("0x0+0+0",&(*image)->page);
                break;
              }
            (void) ResetImagePage(*image,argv[i+1]);
            break;
          }
        if (LocaleCompare("resample",option+1) == 0)
          {
            /*
              Resample image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            mogrify_image=ResampleImage(*image,geometry_info.rho,
              geometry_info.sigma,(*image)->filter,exception);
            break;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            /*
              Resize image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=ResizeImage(*image,geometry.width,geometry.height,
              (*image)->filter,exception);
            break;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            /*
              Roll image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParsePageGeometry(*image,argv[i+1],&geometry,exception);
            if ((flags & PercentValue) != 0)
              {
                geometry.x*=(double) (*image)->columns/100.0;
                geometry.y*=(double) (*image)->rows/100.0;
              }
            mogrify_image=RollImage(*image,geometry.x,geometry.y,exception);
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            char
              *rotation;

            /*
              Check for conditional image rotation.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (strchr(argv[i+1],'>') != (char *) NULL)
              if ((*image)->columns <= (*image)->rows)
                break;
            if (strchr(argv[i+1],'<') != (char *) NULL)
              if ((*image)->columns >= (*image)->rows)
                break;
            /*
              Rotate image.
            */
            rotation=ConstantString(argv[i+1]);
            (void) SubstituteString(&rotation,">","");
            (void) SubstituteString(&rotation,"<","");
            (void) ParseGeometry(rotation,&geometry_info);
            rotation=DestroyString(rotation);
            mogrify_image=RotateImage(*image,geometry_info.rho,exception);
            break;
          }
        break;
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            /*
              Sample image with pixel replication.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=SampleImage(*image,geometry.width,geometry.height,
              exception);
            break;
          }
        if (LocaleCompare("scale",option+1) == 0)
          {
            /*
              Resize image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=ScaleImage(*image,geometry.width,geometry.height,
              exception);
            break;
          }
        if (LocaleCompare("selective-blur",option+1) == 0)
          {
            /*
              Selectively blur pixels within a contrast threshold.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & PercentValue) != 0)
              geometry_info.xi=(double) QuantumRange*geometry_info.xi/100.0;
            mogrify_image=SelectiveBlurImage(*image,geometry_info.rho,
              geometry_info.sigma,geometry_info.xi,exception);
            break;
          }
        if (LocaleCompare("separate",option+1) == 0)
          {
            /*
              Break channels into separate images.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=SeparateImages(*image,exception);
            break;
          }
        if (LocaleCompare("sepia-tone",option+1) == 0)
          {
            double
              threshold;

            /*
              Sepia-tone image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            threshold=StringToDoubleInterval(argv[i+1],(double) QuantumRange+
              1.0);
            mogrify_image=SepiaToneImage(*image,threshold,exception);
            break;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            /*
              Segment image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            (void) SegmentImage(*image,(*image)->colorspace,
              mogrify_info->verbose,geometry_info.rho,geometry_info.sigma,
              exception);
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            char
              *value;

            /*
              Set image option.
            */
            if (*option == '+')
              {
                if (LocaleNCompare(argv[i+1],"registry:",9) == 0)
                  (void) DeleteImageRegistry(argv[i+1]+9);
                else
                  if (LocaleNCompare(argv[i+1],"option:",7) == 0)
                    {
                      (void) DeleteImageOption(mogrify_info,argv[i+1]+7);
                      (void) DeleteImageArtifact(*image,argv[i+1]+7);
                    }
                  else
                    (void) DeleteImageProperty(*image,argv[i+1]);
                break;
              }
            value=InterpretImageProperties(mogrify_info,*image,argv[i+2],
              exception);
            if (value == (char *) NULL)
              break;
            if (LocaleNCompare(argv[i+1],"registry:",9) == 0)
              (void) SetImageRegistry(StringRegistryType,argv[i+1]+9,value,
                exception);
            else
              if (LocaleNCompare(argv[i+1],"option:",7) == 0)
                {
                  (void) SetImageOption(image_info,argv[i+1]+7,value);
                  (void) SetImageOption(mogrify_info,argv[i+1]+7,value);
                  (void) SetImageArtifact(*image,argv[i+1]+7,value);
                }
              else
                (void) SetImageProperty(*image,argv[i+1],value,exception);
            value=DestroyString(value);
            break;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            /*
              Shade image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=ShadeImage(*image,(*option == '-') ? MagickTrue :
              MagickFalse,geometry_info.rho,geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("shadow",option+1) == 0)
          {
            /*
              Shadow image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=4.0;
            if ((flags & PsiValue) == 0)
              geometry_info.psi=4.0;
            mogrify_image=ShadowImage(*image,geometry_info.rho,
              geometry_info.sigma,(ssize_t) ceil(geometry_info.xi-0.5),
              (ssize_t) ceil(geometry_info.psi-0.5),exception);
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            /*
              Sharpen image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=0.0;
            mogrify_image=SharpenImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            /*
              Shave the image edges.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParsePageGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=ShaveImage(*image,&geometry,exception);
            break;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            /*
              Shear image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=geometry_info.rho;
            mogrify_image=ShearImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("sigmoidal-contrast",option+1) == 0)
          {
            /*
              Sigmoidal non-linearity contrast control.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=(double) QuantumRange/2.0;
            if ((flags & PercentValue) != 0)
              geometry_info.sigma=(double) QuantumRange*geometry_info.sigma/
                100.0;
            (void) SigmoidalContrastImage(*image,(*option == '-') ?
              MagickTrue : MagickFalse,geometry_info.rho,geometry_info.sigma,
              exception);
            break;
          }
        if (LocaleCompare("sketch",option+1) == 0)
          {
            /*
              Sketch image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=SketchImage(*image,geometry_info.rho,
              geometry_info.sigma,geometry_info.xi,exception);
            break;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            double
              threshold;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            threshold=StringToDoubleInterval(argv[i+1],(double) QuantumRange+
              1.0);
            (void) SolarizeImage(*image,threshold,exception);
            break;
          }
        if (LocaleCompare("sparse-color",option+1) == 0)
          {
            SparseColorMethod
              method;

            char
              *arguments;

            /*
              Sparse Color Interpolated Gradient
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            method=(SparseColorMethod) ParseCommandOption(
              MagickSparseColorOptions,MagickFalse,argv[i+1]);
            arguments=InterpretImageProperties(mogrify_info,*image,argv[i+2],
              exception);
            if (arguments == (char *) NULL)
              break;
            mogrify_image=SparseColorOption(*image,method,arguments,
              option[0] == '+' ? MagickTrue : MagickFalse,exception);
            arguments=DestroyString(arguments);
            break;
          }
        if (LocaleCompare("splice",option+1) == 0)
          {
            /*
              Splice a solid color into the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseGravityGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=SpliceImage(*image,&geometry,exception);
            break;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            /*
              Spread an image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=SpreadImage(*image,interpolate_method,
              geometry_info.rho,exception);
            break;
          }
        if (LocaleCompare("statistic",option+1) == 0)
          {
            StatisticType
              type;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            type=(StatisticType) ParseCommandOption(MagickStatisticOptions,
              MagickFalse,argv[i+1]);
            (void) ParseGeometry(argv[i+2],&geometry_info);
            mogrify_image=StatisticImage(*image,type,(size_t) geometry_info.rho,
              (size_t) geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("stretch",option+1) == 0)
          {
            if (*option == '+')
              {
                draw_info->stretch=UndefinedStretch;
                break;
              }
            draw_info->stretch=(StretchType) ParseCommandOption(
              MagickStretchOptions,MagickFalse,argv[i+1]);
            break;
          }
        if (LocaleCompare("strip",option+1) == 0)
          {
            /*
              Strip image of profiles and comments.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) StripImage(*image,exception);
            break;
          }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            ExceptionInfo
              *sans;

            PixelInfo
              color;

            if (*option == '+')
              {
                (void) QueryColorCompliance("none",AllCompliance,
                  &draw_info->stroke,exception);
                if (draw_info->stroke_pattern != (Image *) NULL)
                  draw_info->stroke_pattern=DestroyImage(
                    draw_info->stroke_pattern);
                break;
              }
            sans=AcquireExceptionInfo();
            status=QueryColorCompliance(argv[i+1],AllCompliance,&color,sans);
            sans=DestroyExceptionInfo(sans);
            if (status == MagickFalse)
              draw_info->stroke_pattern=GetImageCache(mogrify_info,argv[i+1],
                exception);
            else
              draw_info->stroke=color;
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            draw_info->stroke_width=StringToDouble(argv[i+1],(char **) NULL);
            break;
          }
        if (LocaleCompare("style",option+1) == 0)
          {
            if (*option == '+')
              {
                draw_info->style=UndefinedStyle;
                break;
              }
            draw_info->style=(StyleType) ParseCommandOption(MagickStyleOptions,
              MagickFalse,argv[i+1]);
            break;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            /*
              Swirl image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseGeometry(argv[i+1],&geometry_info);
            mogrify_image=SwirlImage(*image,geometry_info.rho,
              interpolate_method,exception);
            break;
          }
        break;
      }
      case 't':
      {
        if (LocaleCompare("threshold",option+1) == 0)
          {
            double
              threshold;

            /*
              Threshold image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              threshold=(double) QuantumRange/2;
            else
              threshold=StringToDoubleInterval(argv[i+1],(double) QuantumRange+
                1.0);
            (void) BilevelImage(*image,threshold,exception);
            break;
          }
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            /*
              Thumbnail image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) ParseRegionGeometry(*image,argv[i+1],&geometry,exception);
            mogrify_image=ThumbnailImage(*image,geometry.width,geometry.height,
              exception);
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            if (*option == '+')
              {
                if (draw_info->fill_pattern != (Image *) NULL)
                  draw_info->fill_pattern=DestroyImage(draw_info->fill_pattern);
                break;
              }
            draw_info->fill_pattern=GetImageCache(mogrify_info,argv[i+1],
              exception);
            break;
          }
        if (LocaleCompare("tint",option+1) == 0)
          {
            /*
              Tint the image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=TintImage(*image,argv[i+1],&fill,exception);
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          {
            /*
              Affine transform image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=AffineTransformImage(*image,&draw_info->affine,
              exception);
            break;
          }
        if (LocaleCompare("transparent",option+1) == 0)
          {
            PixelInfo
              target;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) QueryColorCompliance(argv[i+1],AllCompliance,&target,
              exception);
            (void) TransparentPaintImage(*image,&target,(Quantum)
              TransparentAlpha,*option == '-' ? MagickFalse : MagickTrue,
              exception);
            break;
          }
        if (LocaleCompare("transpose",option+1) == 0)
          {
            /*
              Transpose image scanlines.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=TransposeImage(*image,exception);
            break;
          }
        if (LocaleCompare("transverse",option+1) == 0)
          {
            /*
              Transverse image scanlines.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=TransverseImage(*image,exception);
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info->tree_depth=StringToUnsignedLong(argv[i+1]);
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          {
            /*
              Trim image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=TrimImage(*image,exception);
            break;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            ImageType
              type;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              type=UndefinedType;
            else
              type=(ImageType) ParseCommandOption(MagickTypeOptions,MagickFalse,
                argv[i+1]);
            (*image)->type=UndefinedType;
            (void) SetImageType(*image,type,exception);
            break;
          }
        break;
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &draw_info->undercolor,exception);
            break;
          }
        if (LocaleCompare("unique",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageArtifact(*image,"identify:unique-colors");
                break;
              }
            (void) SetImageArtifact(*image,"identify:unique-colors","true");
            (void) SetImageArtifact(*image,"verbose","true");
            break;
          }
        if (LocaleCompare("unique-colors",option+1) == 0)
          {
            /*
              Unique image colors.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            mogrify_image=UniqueImageColors(*image,exception);
            break;
          }
        if (LocaleCompare("unsharp",option+1) == 0)
          {
            /*
              Unsharp mask image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=1.0;
            if ((flags & PsiValue) == 0)
              geometry_info.psi=0.05;
            mogrify_image=UnsharpMaskImage(*image,geometry_info.rho,
              geometry_info.sigma,geometry_info.xi,geometry_info.psi,
              exception);
            break;
          }
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            (void) SetImageArtifact(*image,option+1,
              *option == '+' ? "false" : "true");
            break;
          }
        if (LocaleCompare("vignette",option+1) == 0)
          {
            /*
              Vignette image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            if ((flags & XiValue) == 0)
              geometry_info.xi=0.1*(*image)->columns;
            if ((flags & PsiValue) == 0)
              geometry_info.psi=0.1*(*image)->rows;
            if ((flags & PercentValue) != 0)
              {
                geometry_info.xi*=(double) (*image)->columns/100.0;
                geometry_info.psi*=(double) (*image)->rows/100.0;
              }
            mogrify_image=VignetteImage(*image,geometry_info.rho,
              geometry_info.sigma,(ssize_t) ceil(geometry_info.xi-0.5),
              (ssize_t) ceil(geometry_info.psi-0.5),exception);
            break;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageVirtualPixelMethod(*image,
                  UndefinedVirtualPixelMethod,exception);
                break;
              }
            (void) SetImageVirtualPixelMethod(*image,(VirtualPixelMethod)
              ParseCommandOption(MagickVirtualPixelOptions,MagickFalse,
              argv[i+1]),exception);
            break;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            /*
              Wave image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=1.0;
            mogrify_image=WaveImage(*image,geometry_info.rho,
              geometry_info.sigma,interpolate_method,exception);
            break;
          }
        if (LocaleCompare("wavelet-denoise",option+1) == 0)
          {
            /*
              Wavelet denoise image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            flags=ParseGeometry(argv[i+1],&geometry_info);
            if ((flags & PercentValue) != 0)
              {
                geometry_info.rho=QuantumRange*geometry_info.rho/100.0;
                geometry_info.sigma=QuantumRange*geometry_info.sigma/100.0;
              }
            if ((flags & SigmaValue) == 0)
              geometry_info.sigma=0.0;
            mogrify_image=WaveletDenoiseImage(*image,geometry_info.rho,
              geometry_info.sigma,exception);
            break;
          }
        if (LocaleCompare("weight",option+1) == 0)
          {
            ssize_t
              weight;

            weight=ParseCommandOption(MagickWeightOptions,MagickFalse,
              argv[i+1]);
            if (weight == -1)
              weight=(ssize_t) StringToUnsignedLong(argv[i+1]);
            draw_info->weight=(size_t) weight;
            break;
          }
        if (LocaleCompare("white-threshold",option+1) == 0)
          {
            /*
              White threshold image.
            */
            (void) SyncImageSettings(mogrify_info,*image,exception);
            (void) WhiteThresholdImage(*image,argv[i+1],exception);
            break;
          }
        if (LocaleCompare("write-mask",option+1) == 0)
          {
            Image
              *mask;

            (void) SyncImageSettings(mogrify_info,*image,exception);
            if (*option == '+')
              {
                /*
                  Remove a mask.
                */
                (void) SetImageMask(*image,WritePixelMask,(Image *) NULL,
                  exception);
                break;
              }
            /*
              Set the image mask.
            */
            mask=GetImageCache(mogrify_info,argv[i+1],exception);
            if (mask == (Image *) NULL)
              break;
            (void) SetImageMask(*image,WritePixelMask,mask,exception);
            mask=DestroyImage(mask);
            break;
          }
        break;
      }
      default:
        break;
    }
    /*
       Replace current image with any image that was generated
    */
    if (mogrify_image != (Image *) NULL)
      ReplaceImageInListReturnLast(image,mogrify_image);
    i+=count;
  }
  /*
    Free resources.
  */
  quantize_info=DestroyQuantizeInfo(quantize_info);
  draw_info=DestroyDrawInfo(draw_info);
  mogrify_info=DestroyImageInfo(mogrify_info);
  status=(MagickStatusType) (exception->severity < ErrorException ? 1 : 0);
  return(status == 0 ? MagickFalse : MagickTrue);
}