static MagickBooleanType CLISimpleOperatorImage(MagickCLI *cli_wand,
  const char *option, const char *arg1n, const char *arg2n,
  ExceptionInfo *exception)
{
  Image *
    new_image;

  GeometryInfo
    geometry_info;

  RectangleInfo
    geometry;

  MagickStatusType
    flags;

  ssize_t
    parse;

  const char    /* percent escaped versions of the args */
    *arg1,
    *arg2;

#define _image_info       (cli_wand->wand.image_info)
#define _image            (cli_wand->wand.images)
#define _exception        (cli_wand->wand.exception)
#define _draw_info        (cli_wand->draw_info)
#define _quantize_info    (cli_wand->quantize_info)
#define _process_flags    (cli_wand->process_flags)
#define _option_type      ((CommandOptionFlags) cli_wand->command->flags)
#define IfNormalOp        (*option=='-')
#define IfPlusOp          (*option!='-')
#define IsNormalOp        IfNormalOp ? MagickTrue : MagickFalse
#define IsPlusOp          IfNormalOp ? MagickFalse : MagickTrue

  assert(cli_wand != (MagickCLI *) NULL);
  assert(cli_wand->signature == MagickWandSignature);
  assert(cli_wand->wand.signature == MagickWandSignature);
  assert(_image != (Image *) NULL);             /* an image must be present */
  if (cli_wand->wand.debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",cli_wand->wand.name);

  arg1 = arg1n,
  arg2 = arg2n;

  /* Interpret Percent Escapes in Arguments - using first image */
  if ( (((_process_flags & ProcessInterpretProperities) != 0 )
        || ((_option_type & AlwaysInterpretArgsFlag) != 0)
       )  && ((_option_type & NeverInterpretArgsFlag) == 0) ) {
    /* Interpret Percent escapes in argument 1 */
    if (arg1n != (char *) NULL) {
      arg1=InterpretImageProperties(_image_info,_image,arg1n,_exception);
      if (arg1 == (char *) NULL) {
        CLIWandException(OptionWarning,"InterpretPropertyFailure",option);
        arg1=arg1n;  /* use the given argument as is */
      }
    }
    if (arg2n != (char *) NULL) {
      arg2=InterpretImageProperties(_image_info,_image,arg2n,_exception);
      if (arg2 == (char *) NULL) {
        CLIWandException(OptionWarning,"InterpretPropertyFailure",option);
        arg2=arg2n;  /* use the given argument as is */
      }
    }
  }
#undef _process_flags
#undef _option_type

#if 0
  (void) FormatLocaleFile(stderr,
    "CLISimpleOperatorImage: \"%s\" \"%s\" \"%s\"\n",option,arg1,arg2);
#endif

  new_image = (Image *) NULL; /* the replacement image, if not null at end */
  SetGeometryInfo(&geometry_info);

  switch (*(option+1))
  {
    case 'a':
    {
      if (LocaleCompare("adaptive-blur",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=AdaptiveBlurImage(_image,geometry_info.rho,
            geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("adaptive-resize",option+1) == 0)
        {
          /* FUTURE: Roll into a resize special operator */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseRegionGeometry(_image,arg1,&geometry,_exception);
          new_image=AdaptiveResizeImage(_image,geometry.width,geometry.height,
            _exception);
          break;
        }
      if (LocaleCompare("adaptive-sharpen",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=AdaptiveSharpenImage(_image,geometry_info.rho,
            geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("alpha",option+1) == 0)
        {
          parse=ParseCommandOption(MagickAlphaChannelOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedAlphaChannelOption",
              option,arg1);
          (void) SetImageAlphaChannel(_image,(AlphaChannelOption) parse,
            _exception);
          break;
        }
      if (LocaleCompare("annotate",option+1) == 0)
        {
          char
            geometry[MagickPathExtent];

          SetGeometryInfo(&geometry_info);
          flags=ParseGeometry(arg1,&geometry_info);
          if (flags == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho;
          (void) CloneString(&_draw_info->text,arg2);
          (void) FormatLocaleString(geometry,MagickPathExtent,"%+f%+f",
            geometry_info.xi,geometry_info.psi);
          (void) CloneString(&_draw_info->geometry,geometry);
          _draw_info->affine.sx=cos(DegreesToRadians(
            fmod(geometry_info.rho,360.0)));
          _draw_info->affine.rx=sin(DegreesToRadians(
            fmod(geometry_info.rho,360.0)));
          _draw_info->affine.ry=(-sin(DegreesToRadians(
            fmod(geometry_info.sigma,360.0))));
          _draw_info->affine.sy=cos(DegreesToRadians(
            fmod(geometry_info.sigma,360.0)));
          (void) AnnotateImage(_image,_draw_info,_exception);
          GetAffineMatrix(&_draw_info->affine);
          break;
        }
      if (LocaleCompare("auto-gamma",option+1) == 0)
        {
          (void) AutoGammaImage(_image,_exception);
          break;
        }
      if (LocaleCompare("auto-level",option+1) == 0)
        {
          (void) AutoLevelImage(_image,_exception);
          break;
        }
      if (LocaleCompare("auto-orient",option+1) == 0)
        {
          new_image=AutoOrientImage(_image,_image->orientation,_exception);
          break;
        }
      if (LocaleCompare("auto-threshold",option+1) == 0)
        {
          AutoThresholdMethod
            method;

          method=(AutoThresholdMethod) ParseCommandOption(
            MagickAutoThresholdOptions,MagickFalse,arg1);
          (void) AutoThresholdImage(_image,method,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'b':
    {
      if (LocaleCompare("black-threshold",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) BlackThresholdImage(_image,arg1,_exception);
          break;
        }
      if (LocaleCompare("blue-shift",option+1) == 0)
        {
          geometry_info.rho=1.5;
          if (IfNormalOp) {
            flags=ParseGeometry(arg1,&geometry_info);
            if ((flags & RhoValue) == 0)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          }
          new_image=BlueShiftImage(_image,geometry_info.rho,_exception);
          break;
        }
      if (LocaleCompare("blur",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=BlurImage(_image,geometry_info.rho,geometry_info.sigma,
           _exception);
          break;
        }
      if (LocaleCompare("border",option+1) == 0)
        {
          CompositeOperator
            compose;

          const char*
            value;

          flags=ParsePageGeometry(_image,arg1,&geometry,_exception);
          if ((flags & (WidthValue | HeightValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          compose=OverCompositeOp;
          value=GetImageOption(_image_info,"compose");
          if (value != (const char *) NULL)
            compose=(CompositeOperator) ParseCommandOption(MagickComposeOptions,
              MagickFalse,value);
          new_image=BorderImage(_image,&geometry,compose,_exception);
          break;
        }
      if (LocaleCompare("brightness-contrast",option+1) == 0)
        {
          double
            brightness,
            contrast;

          GeometryInfo
            geometry_info;

          MagickStatusType
            flags;

          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          brightness=geometry_info.rho;
          contrast=0.0;
          if ((flags & SigmaValue) != 0)
            contrast=geometry_info.sigma;
          (void) BrightnessContrastImage(_image,brightness,contrast,
            _exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'c':
    {
      if (LocaleCompare("canny",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=10;
          if ((flags & PsiValue) == 0)
            geometry_info.psi=30;
          if ((flags & PercentValue) != 0)
            {
              geometry_info.xi/=100.0;
              geometry_info.psi/=100.0;
            }
          new_image=CannyEdgeImage(_image,geometry_info.rho,geometry_info.sigma,
            geometry_info.xi,geometry_info.psi,_exception);
          break;
        }
      if (LocaleCompare("cdl",option+1) == 0)
        {
          char
            *color_correction_collection; /* Note: arguments do not have percent escapes expanded */

          /*
            Color correct with a color decision list.
          */
          color_correction_collection=FileToString(arg1,~0UL,_exception);
          if (color_correction_collection == (char *) NULL)
            break;
          (void) ColorDecisionListImage(_image,color_correction_collection,
            _exception);
          break;
        }
      if (LocaleCompare("channel",option+1) == 0)
        {
          if (IfPlusOp)
            {
              (void) SetPixelChannelMask(_image,DefaultChannels);
              break;
            }
          parse=ParseChannelOption(arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedChannelType",option,
              arg1);
          (void) SetPixelChannelMask(_image,(ChannelType) parse);
          break;
        }
      if (LocaleCompare("charcoal",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=1.0;
          new_image=CharcoalImage(_image,geometry_info.rho,geometry_info.sigma,
            _exception);
          break;
        }
      if (LocaleCompare("chop",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseGravityGeometry(_image,arg1,&geometry,_exception);
          new_image=ChopImage(_image,&geometry,_exception);
          break;
        }
      if (LocaleCompare("clahe",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParseGeometry(arg1,&geometry_info);
          flags=ParseRegionGeometry(_image,arg1,&geometry,_exception);
          (void) CLAHEImage(_image,geometry.width,geometry.height,
            (size_t) geometry.x,geometry_info.psi,_exception);
          break;
        }
      if (LocaleCompare("clamp",option+1) == 0)
        {
          (void) ClampImage(_image,_exception);
          break;
        }
      if (LocaleCompare("clip",option+1) == 0)
        {
          if (IfNormalOp)
            (void) ClipImage(_image,_exception);
          else /* "+mask" remove the write mask */
            (void) SetImageMask(_image,WritePixelMask,(Image *) NULL,
              _exception);
          break;
        }
      if (LocaleCompare("clip-mask",option+1) == 0)
        {
          Image
            *clip_mask;

          if (IfPlusOp) {
            /* use "+clip-mask" Remove the write mask for -clip-path */
            (void) SetImageMask(_image,WritePixelMask,(Image *) NULL,_exception);
            break;
          }
          clip_mask=GetImageCache(_image_info,arg1,_exception);
          if (clip_mask == (Image *) NULL)
            break;
          (void) SetImageMask(_image,WritePixelMask,clip_mask,_exception);
          clip_mask=DestroyImage(clip_mask);
          break;
        }
      if (LocaleCompare("clip-path",option+1) == 0)
        {
          (void) ClipImagePath(_image,arg1,IsNormalOp,_exception);
          /* Note: Use "+clip-mask" remove the write mask added */
          break;
        }
      if (LocaleCompare("colorize",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=ColorizeImage(_image,arg1,&_draw_info->fill,_exception);
          break;
        }
      if (LocaleCompare("color-matrix",option+1) == 0)
        {
          KernelInfo
            *kernel;

          kernel=AcquireKernelInfo(arg1,exception);
          if (kernel == (KernelInfo *) NULL)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=ColorMatrixImage(_image,kernel,_exception);
          kernel=DestroyKernelInfo(kernel);
          break;
        }
      if (LocaleCompare("colors",option+1) == 0)
        {
          /* Reduce the number of colors in the image.
             FUTURE: also provide 'plus version with image 'color counts'
          */
          _quantize_info->number_colors=StringToUnsignedLong(arg1);
          if (_quantize_info->number_colors == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((_image->storage_class == DirectClass) ||
              _image->colors > _quantize_info->number_colors)
            (void) QuantizeImage(_quantize_info,_image,_exception);
          else
            (void) CompressImageColormap(_image,_exception);
          break;
        }
      if (LocaleCompare("colorspace",option+1) == 0)
        {
          /* WARNING: this is both a image_info setting (already done)
                      and a operator to change image colorspace.

             FUTURE: default colorspace should be sRGB!
             Unless some type of 'linear colorspace' mode is set.

             Note that +colorspace sets "undefined" or no effect on
             new images, but forces images already in memory back to RGB!
             That seems to be a little strange!
          */
          (void) TransformImageColorspace(_image,
                    IfNormalOp ? _image_info->colorspace : sRGBColorspace,
                    _exception);
          break;
        }
      if (LocaleCompare("connected-components",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=ConnectedComponentsImage(_image,(size_t)
            StringToInteger(arg1),(CCObjectInfo **) NULL,_exception);
          break;
        }
      if (LocaleCompare("contrast",option+1) == 0)
        {
          CLIWandWarnReplaced(IfNormalOp?"-level":"+level");
          (void) ContrastImage(_image,IsNormalOp,_exception);
          break;
        }
      if (LocaleCompare("contrast-stretch",option+1) == 0)
        {
          double
            black_point,
            white_point;

          MagickStatusType
            flags;

          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          black_point=geometry_info.rho;
          white_point=(flags & SigmaValue) != 0 ? geometry_info.sigma :
            black_point;
          if ((flags & PercentValue) != 0) {
              black_point*=(double) _image->columns*_image->rows/100.0;
              white_point*=(double) _image->columns*_image->rows/100.0;
            }
          white_point=(double) _image->columns*_image->rows-white_point;
          (void) ContrastStretchImage(_image,black_point,white_point,
            _exception);
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

          kernel_info=AcquireKernelInfo(arg1,exception);
          if (kernel_info == (KernelInfo *) NULL)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          gamma=0.0;
          for (j=0; j < (ssize_t) (kernel_info->width*kernel_info->height); j++)
            gamma+=kernel_info->values[j];
          gamma=1.0/(fabs((double) gamma) <= MagickEpsilon ? 1.0 : gamma);
          for (j=0; j < (ssize_t) (kernel_info->width*kernel_info->height); j++)
            kernel_info->values[j]*=gamma;
          new_image=MorphologyImage(_image,CorrelateMorphology,1,kernel_info,
            _exception);
          kernel_info=DestroyKernelInfo(kernel_info);
          break;
        }
      if (LocaleCompare("crop",option+1) == 0)
        {
          /* WARNING: This can generate multiple images! */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=CropImageToTiles(_image,arg1,_exception);
          break;
        }
      if (LocaleCompare("cycle",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) CycleColormapImage(_image,(ssize_t) StringToLong(arg1),
            _exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'd':
    {
      if (LocaleCompare("decipher",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          StringInfo
            *passkey;

          passkey=FileToStringInfo(arg1,~0UL,_exception);
          if (passkey == (StringInfo *) NULL)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);

          (void) PasskeyDecipherImage(_image,passkey,_exception);
          passkey=DestroyStringInfo(passkey);
          break;
        }
      if (LocaleCompare("depth",option+1) == 0)
        {
          /* The _image_info->depth setting has already been set
             We just need to apply it to all images in current sequence

             WARNING: Depth from 8 to 16 causes 'quantum rounding to images!
             That is it really is an operation, not a setting! Arrgghhh

             FUTURE: this should not be an operator!!!
          */
          (void) SetImageDepth(_image,_image_info->depth,_exception);
          break;
        }
      if (LocaleCompare("deskew",option+1) == 0)
        {
          double
            threshold;

          if (IfNormalOp) {
            if (IsGeometry(arg1) == MagickFalse)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            threshold=StringToDoubleInterval(arg1,(double) QuantumRange+1.0);
          }
          else
            threshold=40.0*QuantumRange/100.0;
          new_image=DeskewImage(_image,threshold,_exception);
          break;
        }
      if (LocaleCompare("despeckle",option+1) == 0)
        {
          new_image=DespeckleImage(_image,_exception);
          break;
        }
      if (LocaleCompare("distort",option+1) == 0)
        {
          double
            *args;

          ssize_t
            count;

          parse = ParseCommandOption(MagickDistortOptions,MagickFalse,arg1);
          if ( parse < 0 )
             CLIWandExceptArgBreak(OptionError,"UnrecognizedDistortMethod",
                                      option,arg1);
          if ((DistortMethod) parse == ResizeDistortion)
            {
               double
                 resize_args[2];
               /* Special Case - Argument is actually a resize geometry!
               ** Convert that to an appropriate distortion argument array.
               ** FUTURE: make a separate special resize operator
                    Roll into a resize special operator */
               if (IsGeometry(arg2) == MagickFalse)
                 CLIWandExceptArgBreak(OptionError,"InvalidGeometry",
                                           option,arg2);
               (void) ParseRegionGeometry(_image,arg2,&geometry,_exception);
               resize_args[0]=(double) geometry.width;
               resize_args[1]=(double) geometry.height;
               new_image=DistortImage(_image,(DistortMethod) parse,
                    (size_t)2,resize_args,MagickTrue,_exception);
               break;
            }
          /* convert argument string into an array of doubles */
          args = StringToArrayOfDoubles(arg2,&count,_exception);
          if (args == (double *) NULL )
            CLIWandExceptArgBreak(OptionError,"InvalidNumberList",option,arg2);

          new_image=DistortImage(_image,(DistortMethod) parse,(size_t)
             count,args,IsPlusOp,_exception);
          args=(double *) RelinquishMagickMemory(args);
          break;
        }
      if (LocaleCompare("draw",option+1) == 0)
        {
          (void) CloneString(&_draw_info->primitive,arg1);
          (void) DrawImage(_image,_draw_info,_exception);
          (void) CloneString(&_draw_info->primitive,(char *) NULL);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'e':
    {
      if (LocaleCompare("edge",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=EdgeImage(_image,geometry_info.rho,_exception);
          break;
        }
      if (LocaleCompare("emboss",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=EmbossImage(_image,geometry_info.rho,
            geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("encipher",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          StringInfo
            *passkey;

          passkey=FileToStringInfo(arg1,~0UL,_exception);
          if (passkey != (StringInfo *) NULL)
            {
              (void) PasskeyEncipherImage(_image,passkey,_exception);
              passkey=DestroyStringInfo(passkey);
            }
          break;
        }
      if (LocaleCompare("enhance",option+1) == 0)
        {
          new_image=EnhanceImage(_image,_exception);
          break;
        }
      if (LocaleCompare("equalize",option+1) == 0)
        {
          (void) EqualizeImage(_image,_exception);
          break;
        }
      if (LocaleCompare("evaluate",option+1) == 0)
        {
          double
            constant;

          parse = ParseCommandOption(MagickEvaluateOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedEvaluateOperator",
                 option,arg1);
          if (IsGeometry(arg2) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg2);
          constant=StringToDoubleInterval(arg2,(double) QuantumRange+1.0);
          (void) EvaluateImage(_image,(MagickEvaluateOperator)parse,constant,
               _exception);
          break;
        }
      if (LocaleCompare("extent",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParseGravityGeometry(_image,arg1,&geometry,_exception);
          if (geometry.width == 0)
            geometry.width=_image->columns;
          if (geometry.height == 0)
            geometry.height=_image->rows;
          new_image=ExtentImage(_image,&geometry,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'f':
    {
      if (LocaleCompare("flip",option+1) == 0)
        {
          new_image=FlipImage(_image,_exception);
          break;
        }
      if (LocaleCompare("flop",option+1) == 0)
        {
          new_image=FlopImage(_image,_exception);
          break;
        }
      if (LocaleCompare("floodfill",option+1) == 0)
        {
          PixelInfo
            target;

          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParsePageGeometry(_image,arg1,&geometry,_exception);
          (void) QueryColorCompliance(arg2,AllCompliance,&target,_exception);
          (void) FloodfillPaintImage(_image,_draw_info,&target,geometry.x,
            geometry.y,IsPlusOp,_exception);
          break;
        }
      if (LocaleCompare("frame",option+1) == 0)
        {
          FrameInfo
            frame_info;

          CompositeOperator
            compose;

          const char*
            value;

          value=GetImageOption(_image_info,"compose");
            compose=OverCompositeOp;  /* use Over not _image->compose */
          if (value != (const char *) NULL)
            compose=(CompositeOperator) ParseCommandOption(MagickComposeOptions,
              MagickFalse,value);
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParsePageGeometry(_image,arg1,&geometry,_exception);
          frame_info.width=geometry.width;
          frame_info.height=geometry.height;
          frame_info.outer_bevel=geometry.x;
          frame_info.inner_bevel=geometry.y;
          frame_info.x=(ssize_t) frame_info.width;
          frame_info.y=(ssize_t) frame_info.height;
          frame_info.width=_image->columns+2*frame_info.width;
          frame_info.height=_image->rows+2*frame_info.height;
          new_image=FrameImage(_image,&frame_info,compose,_exception);
          break;
        }
      if (LocaleCompare("function",option+1) == 0)
        {
          double
            *args;

          ssize_t
            count;

          parse=ParseCommandOption(MagickFunctionOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedFunction",
                 option,arg1);
          /* convert argument string into an array of doubles */
          args = StringToArrayOfDoubles(arg2,&count,_exception);
          if (args == (double *) NULL )
            CLIWandExceptArgBreak(OptionError,"InvalidNumberList",option,arg2);

          (void) FunctionImage(_image,(MagickFunction)parse,(size_t) count,args,
               _exception);
          args=(double *) RelinquishMagickMemory(args);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'g':
    {
      if (LocaleCompare("gamma",option+1) == 0)
        {
          double
            constant;

          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          constant=StringToDouble(arg1,(char **) NULL);
#if 0
          /* Using Gamma, via a cache */
          if (IfPlusOp)
            constant=PerceptibleReciprocal(constant);
          (void) GammaImage(_image,constant,_exception);
#else
          /* Using Evaluate POW, direct update of values - more accurite */
          if (IfNormalOp)
            constant=PerceptibleReciprocal(constant);
          (void) EvaluateImage(_image,PowEvaluateOperator,constant,_exception);
          _image->gamma*=StringToDouble(arg1,(char **) NULL);
#endif
          /* Set gamma setting -- Old meaning of "+gamma"
           * _image->gamma=StringToDouble(arg1,(char **) NULL);
           */
          break;
        }
      if (LocaleCompare("gaussian-blur",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=GaussianBlurImage(_image,geometry_info.rho,
            geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("gaussian",option+1) == 0)
        {
          CLIWandWarnReplaced("-gaussian-blur");
          (void) CLISimpleOperatorImage(cli_wand,"-gaussian-blur",arg1,NULL,exception);
        }
      if (LocaleCompare("geometry",option+1) == 0)
        {
          /*
            Record Image offset for composition. (A Setting)
            Resize last _image. (ListOperator)  -- DEPRECIATE
            FUTURE: Why if no 'offset' does this resize ALL images?
            Also why is the setting recorded in the IMAGE non-sense!
          */
          if (IfPlusOp)
            { /* remove the previous composition geometry offset! */
              if (_image->geometry != (char *) NULL)
                _image->geometry=DestroyString(_image->geometry);
              break;
            }
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParseRegionGeometry(_image,arg1,&geometry,_exception);
          if (((flags & XValue) != 0) || ((flags & YValue) != 0))
            (void) CloneString(&_image->geometry,arg1);
          else
            new_image=ResizeImage(_image,geometry.width,geometry.height,
              _image->filter,_exception);
          break;
        }
      if (LocaleCompare("grayscale",option+1) == 0)
        {
          parse=ParseCommandOption(MagickPixelIntensityOptions,
            MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedIntensityMethod",
              option,arg1);
          (void) GrayscaleImage(_image,(PixelIntensityMethod) parse,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'h':
    {
      if (LocaleCompare("hough-lines",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho;
          if ((flags & XiValue) == 0)
            geometry_info.xi=40;
          new_image=HoughLineImage(_image,(size_t) geometry_info.rho,
            (size_t) geometry_info.sigma,(size_t) geometry_info.xi,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'i':
    {
      if (LocaleCompare("identify",option+1) == 0)
        {
          const char
            *format,
            *text;

          format=GetImageOption(_image_info,"format");
          if (format == (char *) NULL)
            {
              (void) IdentifyImage(_image,stdout,_image_info->verbose,
                _exception);
              break;
            }
          text=InterpretImageProperties(_image_info,_image,format,_exception);
          if (text == (char *) NULL)
            CLIWandExceptionBreak(OptionWarning,"InterpretPropertyFailure",
              option);
          (void) fputs(text,stdout);
          text=DestroyString((char *)text);
          break;
        }
      if (LocaleCompare("implode",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=ImplodeImage(_image,geometry_info.rho,_image->interpolate,
               _exception);
          break;
        }
      if (LocaleCompare("interpolative-resize",option+1) == 0)
        {
          /* FUTURE: New to IMv7
               Roll into a resize special operator */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseRegionGeometry(_image,arg1,&geometry,_exception);
          new_image=InterpolativeResizeImage(_image,geometry.width,
               geometry.height,_image->interpolate,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'k':
    {
      if (LocaleCompare("kuwahara",option+1) == 0)
        {
          /*
            Edge preserving blur.
          */
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho-0.5;
          new_image=KuwaharaImage(_image,geometry_info.rho,geometry_info.sigma,
           _exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'l':
    {
      if (LocaleCompare("lat",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & PercentValue) != 0)
            geometry_info.xi=(double) QuantumRange*geometry_info.xi/100.0;
          new_image=AdaptiveThresholdImage(_image,(size_t) geometry_info.rho,
               (size_t) geometry_info.sigma,(double) geometry_info.xi,
               _exception);
          break;
        }
      if (LocaleCompare("level",option+1) == 0)
        {
          double
            black_point,
            gamma,
            white_point;

          MagickStatusType
            flags;

          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
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
          if (IfPlusOp || ((flags & AspectValue) != 0))
            (void) LevelizeImage(_image,black_point,white_point,gamma,_exception);
          else
            (void) LevelImage(_image,black_point,white_point,gamma,_exception);
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

          p=(const char *) arg1;
          GetNextToken(p,&p,MagickPathExtent,token);  /* get black point color */
          if ((isalpha((int) *token) != 0) || ((*token == '#') != 0))
            (void) QueryColorCompliance(token,AllCompliance,
                      &black_point,_exception);
          else
            (void) QueryColorCompliance("#000000",AllCompliance,
                      &black_point,_exception);
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
                           &white_point,_exception);
              else
                (void) QueryColorCompliance("#ffffff",AllCompliance,
                           &white_point,_exception);
            }
          (void) LevelImageColors(_image,&black_point,&white_point,
                     IsPlusOp,_exception);
          break;
        }
      if (LocaleCompare("linear-stretch",option+1) == 0)
        {
          double
            black_point,
            white_point;

          MagickStatusType
            flags;

          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          black_point=geometry_info.rho;
          white_point=(double) _image->columns*_image->rows;
          if ((flags & SigmaValue) != 0)
            white_point=geometry_info.sigma;
          if ((flags & PercentValue) != 0)
            {
              black_point*=(double) _image->columns*_image->rows/100.0;
              white_point*=(double) _image->columns*_image->rows/100.0;
            }
          if ((flags & SigmaValue) == 0)
            white_point=(double) _image->columns*_image->rows-
              black_point;
          (void) LinearStretchImage(_image,black_point,white_point,_exception);
          break;
        }
      if (LocaleCompare("liquid-rescale",option+1) == 0)
        {
          /* FUTURE: Roll into a resize special operator */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParseRegionGeometry(_image,arg1,&geometry,_exception);
          if ((flags & XValue) == 0)
            geometry.x=1;
          if ((flags & YValue) == 0)
            geometry.y=0;
          new_image=LiquidRescaleImage(_image,geometry.width,
            geometry.height,1.0*geometry.x,1.0*geometry.y,_exception);
          break;
        }
      if (LocaleCompare("local-contrast",option+1) == 0)
        {
          MagickStatusType
            flags;

          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            geometry_info.rho=10;
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=12.5;
          new_image=LocalContrastImage(_image,geometry_info.rho,
            geometry_info.sigma,exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'm':
    {
      if (LocaleCompare("magnify",option+1) == 0)
        {
          new_image=MagnifyImage(_image,_exception);
          break;
        }
      if (LocaleCompare("map",option+1) == 0)
        {
          CLIWandWarnReplaced("-remap");
          (void) CLISimpleOperatorImage(cli_wand,"-remap",NULL,NULL,exception);
          break;
        }
      if (LocaleCompare("mask",option+1) == 0)
        {
          Image
            *mask;

          if (IfPlusOp)
            {
              /*
                Remove a mask.
              */
              (void) SetImageMask(_image,WritePixelMask,(Image *) NULL,
                _exception);
              break;
            }
          /*
            Set the image mask.
          */
          mask=GetImageCache(_image_info,arg1,_exception);
          if (mask == (Image *) NULL)
            break;
          (void) SetImageMask(_image,WritePixelMask,mask,_exception);
          mask=DestroyImage(mask);
          break;
        }
      if (LocaleCompare("matte",option+1) == 0)
        {
          CLIWandWarnReplaced(IfNormalOp?"-alpha Set":"-alpha Off");
          (void) SetImageAlphaChannel(_image,IfNormalOp ? SetAlphaChannel :
            DeactivateAlphaChannel, _exception);
          break;
        }
      if (LocaleCompare("mean-shift",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=0.10*QuantumRange;
          if ((flags & PercentValue) != 0)
            geometry_info.xi=(double) QuantumRange*geometry_info.xi/100.0;
          new_image=MeanShiftImage(_image,(size_t) geometry_info.rho,
            (size_t) geometry_info.sigma,geometry_info.xi,_exception);
          break;
        }
      if (LocaleCompare("median",option+1) == 0)
        {
          CLIWandWarnReplaced("-statistic Median");
          (void) CLISimpleOperatorImage(cli_wand,"-statistic","Median",arg1,exception);
          break;
        }
      if (LocaleCompare("mode",option+1) == 0)
        {
          /* FUTURE: note this is also a special "montage" option */
          CLIWandWarnReplaced("-statistic Mode");
          (void) CLISimpleOperatorImage(cli_wand,"-statistic","Mode",arg1,exception);
          break;
        }
      if (LocaleCompare("modulate",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ModulateImage(_image,arg1,_exception);
          break;
        }
      if (LocaleCompare("monitor",option+1) == 0)
        {
          (void) SetImageProgressMonitor(_image, IfNormalOp ? MonitorProgress :
                (MagickProgressMonitor) NULL,(void *) NULL);
          break;
        }
      if (LocaleCompare("monochrome",option+1) == 0)
        {
          (void) SetImageType(_image,BilevelType,_exception);
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

          ssize_t
            iterations;

          p=arg1;
          GetNextToken(p,&p,MagickPathExtent,token);
          parse=ParseCommandOption(MagickMorphologyOptions,MagickFalse,token);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedFunction",option,
              arg1);
          iterations=1L;
          GetNextToken(p,&p,MagickPathExtent,token);
          if ((*p == ':') || (*p == ','))
            GetNextToken(p,&p,MagickPathExtent,token);
          if ((*p != '\0'))
            iterations=(ssize_t) StringToLong(p);
          kernel=AcquireKernelInfo(arg2,exception);
          if (kernel == (KernelInfo *) NULL)
            CLIWandExceptArgBreak(OptionError,"UnabletoParseKernel",option,arg2);
          new_image=MorphologyImage(_image,(MorphologyMethod)parse,iterations,
            kernel,_exception);
          kernel=DestroyKernelInfo(kernel);
          break;
        }
      if (LocaleCompare("motion-blur",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=MotionBlurImage(_image,geometry_info.rho,geometry_info.sigma,
            geometry_info.xi,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'n':
    {
      if (LocaleCompare("negate",option+1) == 0)
        {
          (void) NegateImage(_image, IsPlusOp, _exception);
          break;
        }
      if (LocaleCompare("noise",option+1) == 0)
        {
          double
            attenuate;

          const char*
            value;

          if (IfNormalOp)
            {
              CLIWandWarnReplaced("-statistic NonPeak");
              (void) CLISimpleOperatorImage(cli_wand,"-statistic","NonPeak",arg1,exception);
              break;
            }
          parse=ParseCommandOption(MagickNoiseOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedNoiseType",
                option,arg1);
          attenuate=1.0;
          value=GetImageOption(_image_info,"attenuate");
          if  (value != (const char *) NULL)
            attenuate=StringToDouble(value,(char **) NULL);
          new_image=AddNoiseImage(_image,(NoiseType)parse,attenuate,
               _exception);
          break;
        }
      if (LocaleCompare("normalize",option+1) == 0)
        {
          (void) NormalizeImage(_image,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'o':
    {
      if (LocaleCompare("opaque",option+1) == 0)
        {
          PixelInfo
            target;

          (void) QueryColorCompliance(arg1,AllCompliance,&target,_exception);
          (void) OpaquePaintImage(_image,&target,&_draw_info->fill,IsPlusOp,
               _exception);
          break;
        }
      if (LocaleCompare("ordered-dither",option+1) == 0)
        {
          (void) OrderedDitherImage(_image,arg1,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'p':
    {
      if (LocaleCompare("paint",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=OilPaintImage(_image,geometry_info.rho,geometry_info.sigma,
               _exception);
          break;
        }
      if (LocaleCompare("perceptible",option+1) == 0)
        {
          (void) PerceptibleImage(_image,StringToDouble(arg1,(char **) NULL),
            _exception);
          break;
        }
      if (LocaleCompare("polaroid",option+1) == 0)
        {
          const char
            *caption;

          double
            angle;

          if (IfPlusOp) {
            RandomInfo
              *random_info;

            random_info=AcquireRandomInfo();
            angle=22.5*(GetPseudoRandomValue(random_info)-0.5);
            random_info=DestroyRandomInfo(random_info);
          }
          else {
            flags=ParseGeometry(arg1,&geometry_info);
            if ((flags & RhoValue) == 0)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            angle=geometry_info.rho;
          }
          caption=GetImageProperty(_image,"caption",_exception);
          new_image=PolaroidImage(_image,_draw_info,caption,angle,
            _image->interpolate,_exception);
          break;
        }
      if (LocaleCompare("posterize",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) PosterizeImage(_image,(size_t) geometry_info.rho,
            _quantize_info->dither_method,_exception);
          break;
        }
      if (LocaleCompare("preview",option+1) == 0)
        {
          /* FUTURE: should be a 'Genesis' option?
             Option however is also in WandSettingOptionInfo()
             Why???
          */
          parse=ParseCommandOption(MagickPreviewOptions, MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedPreviewType",
                option,arg1);
          new_image=PreviewImage(_image,(PreviewType)parse,_exception);
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

          /* Note: arguments do not have percent escapes expanded */
          if (IfPlusOp)
            { /* Remove a profile from the _image.  */
              (void) ProfileImage(_image,arg1,(const unsigned char *)
                NULL,0,_exception);
              break;
            }
          /* Associate a profile with the _image.  */
          profile_info=CloneImageInfo(_image_info);
          profile=GetImageProfile(_image,"iptc");
          if (profile != (StringInfo *) NULL)
            profile_info->profile=(void *) CloneStringInfo(profile);
          profile_image=GetImageCache(profile_info,arg1,_exception);
          profile_info=DestroyImageInfo(profile_info);
          if (profile_image == (Image *) NULL)
            {
              StringInfo
                *profile;

              profile_info=CloneImageInfo(_image_info);
              (void) CopyMagickString(profile_info->filename,arg1,
                MagickPathExtent);
              profile=FileToStringInfo(profile_info->filename,~0UL,_exception);
              if (profile != (StringInfo *) NULL)
                {
                  (void) SetImageInfo(profile_info,0,_exception);
                  (void) ProfileImage(_image,profile_info->magick,
                    GetStringInfoDatum(profile),(size_t)
                    GetStringInfoLength(profile),_exception);
                  profile=DestroyStringInfo(profile);
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
              (void) ProfileImage(_image,name,GetStringInfoDatum(profile),
                (size_t) GetStringInfoLength(profile),_exception);
            name=GetNextImageProfile(profile_image);
          }
          profile_image=DestroyImage(profile_image);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'r':
    {
      if (LocaleCompare("raise",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParsePageGeometry(_image,arg1,&geometry,_exception);
          (void) RaiseImage(_image,&geometry,IsNormalOp,_exception);
          break;
        }
      if (LocaleCompare("random-threshold",option+1) == 0)
        {
          double
            min_threshold,
            max_threshold;

          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          min_threshold=0.0;
          max_threshold=(double) QuantumRange;
          flags=ParseGeometry(arg1,&geometry_info);
          min_threshold=geometry_info.rho;
          max_threshold=geometry_info.sigma;
          if ((flags & SigmaValue) == 0)
            max_threshold=min_threshold;
          if (strchr(arg1,'%') != (char *) NULL)
            {
              max_threshold*=(double) (0.01*QuantumRange);
              min_threshold*=(double) (0.01*QuantumRange);
            }
          (void) RandomThresholdImage(_image,min_threshold,max_threshold,
            _exception);
          break;
        }
      if (LocaleCompare("range-threshold",option+1) == 0)
        {
          /*
            Range threshold image.
          */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho;
          if ((flags & XiValue) == 0)
            geometry_info.xi=geometry_info.sigma;
          if ((flags & PsiValue) == 0)
            geometry_info.psi=geometry_info.xi;
          if (strchr(arg1,'%') != (char *) NULL)
            {
              geometry_info.rho*=(double) (0.01*QuantumRange);
              geometry_info.sigma*=(double) (0.01*QuantumRange);
              geometry_info.xi*=(double) (0.01*QuantumRange);
              geometry_info.psi*=(double) (0.01*QuantumRange);
            }
          (void) RangeThresholdImage(_image,geometry_info.rho,
            geometry_info.sigma,geometry_info.xi,geometry_info.psi,exception);
          break;
        }
      if (LocaleCompare("read-mask",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          Image
            *mask;

          if (IfPlusOp)
            { /* Remove a mask. */
              (void) SetImageMask(_image,ReadPixelMask,(Image *) NULL,
                _exception);
              break;
            }
          /* Set the image mask. */
          mask=GetImageCache(_image_info,arg1,_exception);
          if (mask == (Image *) NULL)
            break;
          (void) SetImageMask(_image,ReadPixelMask,mask,_exception);
          mask=DestroyImage(mask);
          break;
        }
      if (LocaleCompare("recolor",option+1) == 0)
        {
          CLIWandWarnReplaced("-color-matrix");
          (void) CLISimpleOperatorImage(cli_wand,"-color-matrix",arg1,NULL,
            exception);
        }
      if (LocaleCompare("region",option+1) == 0)
        {
          if (*option == '+')
            {
              (void) SetImageRegionMask(_image,WritePixelMask,
                (const RectangleInfo *) NULL,_exception);
              break;
            }
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseGravityGeometry(_image,arg1,&geometry,_exception);
          (void) SetImageRegionMask(_image,WritePixelMask,&geometry,_exception);
          break;
        }
      if (LocaleCompare("remap",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          Image
            *remap_image;

          remap_image=GetImageCache(_image_info,arg1,_exception);
          if (remap_image == (Image *) NULL)
            break;
          (void) RemapImage(_quantize_info,_image,remap_image,_exception);
          remap_image=DestroyImage(remap_image);
          break;
        }
      if (LocaleCompare("repage",option+1) == 0)
        {
          if (IfNormalOp)
            {
              if (IsGeometry(arg1) == MagickFalse)
                CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,
                  arg1);
              (void) ResetImagePage(_image,arg1);
            }
          else
            (void) ParseAbsoluteGeometry("0x0+0+0",&_image->page);
          break;
        }
      if (LocaleCompare("resample",option+1) == 0)
        {
          /* FUTURE: Roll into a resize special operation */
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho;
          new_image=ResampleImage(_image,geometry_info.rho,
            geometry_info.sigma,_image->filter,_exception);
          break;
        }
      if (LocaleCompare("resize",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseRegionGeometry(_image,arg1,&geometry,_exception);
          new_image=ResizeImage(_image,geometry.width,geometry.height,
            _image->filter,_exception);
          break;
        }
      if (LocaleCompare("roll",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParsePageGeometry(_image,arg1,&geometry,_exception);
          if ((flags & PercentValue) != 0)
            {
              geometry.x*=(double) _image->columns/100.0;
              geometry.y*=(double) _image->rows/100.0;
            }
          new_image=RollImage(_image,geometry.x,geometry.y,_exception);
          break;
        }
      if (LocaleCompare("rotate",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & GreaterValue) != 0 && (_image->columns <= _image->rows))
            break;
          if ((flags & LessValue) != 0 && (_image->columns >= _image->rows))
            break;
          new_image=RotateImage(_image,geometry_info.rho,_exception);
          break;
        }
      if (LocaleCompare("rotational-blur",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=RotationalBlurImage(_image,geometry_info.rho,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 's':
    {
      if (LocaleCompare("sample",option+1) == 0)
        {
          /* FUTURE: Roll into a resize special operator */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseRegionGeometry(_image,arg1,&geometry,_exception);
          new_image=SampleImage(_image,geometry.width,geometry.height,
            _exception);
          break;
        }
      if (LocaleCompare("scale",option+1) == 0)
        {
          /* FUTURE: Roll into a resize special operator */
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseRegionGeometry(_image,arg1,&geometry,_exception);
          new_image=ScaleImage(_image,geometry.width,geometry.height,
            _exception);
          break;
        }
      if (LocaleCompare("segment",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          (void) SegmentImage(_image,_image->colorspace,
            _image_info->verbose,geometry_info.rho,geometry_info.sigma,
            _exception);
          break;
        }
      if (LocaleCompare("selective-blur",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & PercentValue) != 0)
            geometry_info.xi=(double) QuantumRange*geometry_info.xi/100.0;
          new_image=SelectiveBlurImage(_image,geometry_info.rho,
            geometry_info.sigma,geometry_info.xi,_exception);
          break;
        }
      if (LocaleCompare("separate",option+1) == 0)
        {
          /* WARNING: This can generate multiple images! */
          /* FUTURE - this may be replaced by a "-channel" method */
          new_image=SeparateImages(_image,_exception);
          break;
        }
      if (LocaleCompare("sepia-tone",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=SepiaToneImage(_image,StringToDoubleInterval(arg1,
                 (double) QuantumRange+1.0),_exception);
          break;
        }
      if (LocaleCompare("shade",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if (((flags & RhoValue) == 0) || ((flags & SigmaValue) == 0))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=ShadeImage(_image,IsNormalOp,geometry_info.rho,
               geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("shadow",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=4.0;
          if ((flags & PsiValue) == 0)
            geometry_info.psi=4.0;
          new_image=ShadowImage(_image,geometry_info.rho,geometry_info.sigma,
            (ssize_t) ceil(geometry_info.xi-0.5),(ssize_t)
            ceil(geometry_info.psi-0.5),_exception);
          break;
        }
      if (LocaleCompare("sharpen",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=0.0;
          new_image=SharpenImage(_image,geometry_info.rho,geometry_info.sigma,
           _exception);
          break;
        }
      if (LocaleCompare("shave",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParsePageGeometry(_image,arg1,&geometry,_exception);
          new_image=ShaveImage(_image,&geometry,_exception);
          break;
        }
      if (LocaleCompare("shear",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho;
          new_image=ShearImage(_image,geometry_info.rho,geometry_info.sigma,
            _exception);
          break;
        }
      if (LocaleCompare("sigmoidal-contrast",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=(double) QuantumRange/2.0;
          if ((flags & PercentValue) != 0)
            geometry_info.sigma=(double) QuantumRange*geometry_info.sigma/
              100.0;
          (void) SigmoidalContrastImage(_image,IsNormalOp,geometry_info.rho,
               geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("sketch",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=SketchImage(_image,geometry_info.rho,
            geometry_info.sigma,geometry_info.xi,_exception);
          break;
        }
      if (LocaleCompare("solarize",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SolarizeImage(_image,StringToDoubleInterval(arg1,(double)
                 QuantumRange+1.0),_exception);
          break;
        }
      if (LocaleCompare("sparse-color",option+1) == 0)
        {
          parse= ParseCommandOption(MagickSparseColorOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedSparseColorMethod",
                option,arg1);
          new_image=SparseColorOption(_image,(SparseColorMethod)parse,arg2,
               _exception);
          break;
        }
      if (LocaleCompare("splice",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          flags=ParseGravityGeometry(_image,arg1,&geometry,_exception);
          new_image=SpliceImage(_image,&geometry,_exception);
          break;
        }
      if (LocaleCompare("spread",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg2);
          new_image=SpreadImage(_image,_image->interpolate,geometry_info.rho,
           _exception);
          break;
        }
      if (LocaleCompare("statistic",option+1) == 0)
        {
          parse=ParseCommandOption(MagickStatisticOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedStatisticType",
                 option,arg1);
          flags=ParseGeometry(arg2,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg2);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=geometry_info.rho;
          new_image=StatisticImage(_image,(StatisticType)parse,
               (size_t) geometry_info.rho,(size_t) geometry_info.sigma,
               _exception);
          break;
        }
      if (LocaleCompare("strip",option+1) == 0)
        {
          (void) StripImage(_image,_exception);
          break;
        }
      if (LocaleCompare("swirl",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=SwirlImage(_image,geometry_info.rho,
            _image->interpolate,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 't':
    {
      if (LocaleCompare("threshold",option+1) == 0)
        {
          double
            threshold;

          threshold=(double) QuantumRange/2;
          if (IfNormalOp) {
            if (IsGeometry(arg1) == MagickFalse)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            threshold=StringToDoubleInterval(arg1,(double) QuantumRange+1.0);
          }
          (void) BilevelImage(_image,threshold,_exception);
          break;
        }
      if (LocaleCompare("thumbnail",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) ParseRegionGeometry(_image,arg1,&geometry,_exception);
          new_image=ThumbnailImage(_image,geometry.width,geometry.height,
            _exception);
          break;
        }
      if (LocaleCompare("tint",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          new_image=TintImage(_image,arg1,&_draw_info->fill,_exception);
          break;
        }
      if (LocaleCompare("transform",option+1) == 0)
        {
          CLIWandWarnReplaced("+distort AffineProjection");
          new_image=AffineTransformImage(_image,&_draw_info->affine,_exception);
          break;
        }
      if (LocaleCompare("transparent",option+1) == 0)
        {
          PixelInfo
            target;

          (void) QueryColorCompliance(arg1,AllCompliance,&target,_exception);
          (void) TransparentPaintImage(_image,&target,(Quantum)
            TransparentAlpha,IsPlusOp,_exception);
          break;
        }
      if (LocaleCompare("transpose",option+1) == 0)
        {
          new_image=TransposeImage(_image,_exception);
          break;
        }
      if (LocaleCompare("transverse",option+1) == 0)
        {
          new_image=TransverseImage(_image,_exception);
          break;
        }
      if (LocaleCompare("trim",option+1) == 0)
        {
          new_image=TrimImage(_image,_exception);
          break;
        }
      if (LocaleCompare("type",option+1) == 0)
        {
          /* Note that "type" setting should have already been defined */
          (void) SetImageType(_image,_image_info->type,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'u':
    {
      if (LocaleCompare("unique",option+1) == 0)
        {
          /* FUTURE: move to SyncImageSettings() and AcqireImage()???
             Option is not documented, bt appears to be for "identify".
             We may need a identify specific verbose!
          */
          if (IsPlusOp) {
              (void) DeleteImageArtifact(_image,"identify:unique-colors");
              break;
            }
          (void) SetImageArtifact(_image,"identify:unique-colors","true");
          (void) SetImageArtifact(_image,"verbose","true");
          break;
        }
      if (LocaleCompare("unique-colors",option+1) == 0)
        {
          new_image=UniqueImageColors(_image,_exception);
          break;
        }
      if (LocaleCompare("unsharp",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=1.0;
          if ((flags & PsiValue) == 0)
            geometry_info.psi=0.05;
          new_image=UnsharpMaskImage(_image,geometry_info.rho,
            geometry_info.sigma,geometry_info.xi,geometry_info.psi,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'v':
    {
      if (LocaleCompare("verbose",option+1) == 0)
        {
          /* FUTURE: move to SyncImageSettings() and AcquireImage()???
             three places!   ImageArtifact   ImageOption  _image_info->verbose
             Some how new images also get this artifact!
          */
          (void) SetImageArtifact(_image,option+1,
                           IfNormalOp ? "true" : "false" );
          break;
        }
      if (LocaleCompare("vignette",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          if ((flags & XiValue) == 0)
            geometry_info.xi=0.1*_image->columns;
          if ((flags & PsiValue) == 0)
            geometry_info.psi=0.1*_image->rows;
          if ((flags & PercentValue) != 0)
            {
              geometry_info.xi*=(double) _image->columns/100.0;
              geometry_info.psi*=(double) _image->rows/100.0;
            }
          new_image=VignetteImage(_image,geometry_info.rho,geometry_info.sigma,
            (ssize_t) ceil(geometry_info.xi-0.5),(ssize_t)
            ceil(geometry_info.psi-0.5),_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'w':
    {
      if (LocaleCompare("wave",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & (RhoValue|SigmaValue)) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=1.0;
          new_image=WaveImage(_image,geometry_info.rho,geometry_info.sigma,
            _image->interpolate,_exception);
          break;
        }
      if (LocaleCompare("wavelet-denoise",option+1) == 0)
        {
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          if ((flags & PercentValue) != 0)
            {
              geometry_info.rho=QuantumRange*geometry_info.rho/100.0;
              geometry_info.sigma=QuantumRange*geometry_info.sigma/100.0;
            }
          if ((flags & SigmaValue) == 0)
            geometry_info.sigma=0.0;
          new_image=WaveletDenoiseImage(_image,geometry_info.rho,
            geometry_info.sigma,_exception);
          break;
        }
      if (LocaleCompare("white-threshold",option+1) == 0)
        {
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) WhiteThresholdImage(_image,arg1,_exception);
          break;
        }
      if (LocaleCompare("write-mask",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          Image
            *mask;

          if (IfPlusOp)
            { /* Remove a mask. */
              (void) SetImageMask(_image,WritePixelMask,(Image *) NULL,
                _exception);
              break;
            }
          /* Set the image mask. */
          mask=GetImageCache(_image_info,arg1,_exception);
          if (mask == (Image *) NULL)
            break;
          (void) SetImageMask(_image,WritePixelMask,mask,_exception);
          mask=DestroyImage(mask);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    default:
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
  }
  /* clean up percent escape interpreted strings */
  if (arg1 != arg1n )
    arg1=DestroyString((char *)arg1);
  if (arg2 != arg2n )
    arg2=DestroyString((char *)arg2);

  /* Replace current image with any image that was generated
     and set image point to last image (so image->next is correct) */
  if (new_image != (Image *) NULL)
    ReplaceImageInListReturnLast(&_image,new_image);

  return(MagickTrue);
#undef _image_info
#undef _draw_info
#undef _quantize_info
#undef _image
#undef _exception
#undef IfNormalOp
#undef IfPlusOp
#undef IsNormalOp
#undef IsPlusOp
}