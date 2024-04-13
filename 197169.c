WandPrivate void CLISettingOptionInfo(MagickCLI *cli_wand,
     const char *option,const char *arg1n, const char *arg2n)
{
  ssize_t
    parse;     /* option argument parsing (string to value table lookup) */

  const char    /* percent escaped versions of the args */
    *arg1,
    *arg2;

#define _image_info       (cli_wand->wand.image_info)
#define _image            (cli_wand->wand.images)
#define _exception        (cli_wand->wand.exception)
#define _draw_info        (cli_wand->draw_info)
#define _quantize_info    (cli_wand->quantize_info)
#define IfSetOption       (*option=='-')
#define ArgBoolean        IfSetOption ? MagickTrue : MagickFalse
#define ArgBooleanNot     IfSetOption ? MagickFalse : MagickTrue
#define ArgBooleanString  (IfSetOption?"true":"false")
#define ArgOption(def)    (IfSetOption?arg1:(const char *)(def))

  assert(cli_wand != (MagickCLI *) NULL);
  assert(cli_wand->signature == MagickWandSignature);
  assert(cli_wand->wand.signature == MagickWandSignature);

  if (cli_wand->wand.debug != MagickFalse)
    (void) CLILogEvent(cli_wand,CommandEvent,GetMagickModule(),
         "- Setting Option: %s \"%s\" \"%s\"", option,arg1n,arg2n);

  arg1 = arg1n,
  arg2 = arg2n;

#if 1
#define _process_flags    (cli_wand->process_flags)
#define _option_type      ((CommandOptionFlags) cli_wand->command->flags)
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
#endif

  switch (*(option+1))
  {
    case 'a':
    {
      if (LocaleCompare("adjoin",option+1) == 0)
        {
          _image_info->adjoin = ArgBoolean;
          break;
        }
      if (LocaleCompare("affine",option+1) == 0)
        {
          CLIWandWarnReplaced("-draw 'affine ...'");
          if (IfSetOption)
            (void) ParseAffineGeometry(arg1,&_draw_info->affine,_exception);
          else
            GetAffineMatrix(&_draw_info->affine);
          break;
        }
      if (LocaleCompare("antialias",option+1) == 0)
        {
          _image_info->antialias =
            _draw_info->stroke_antialias =
              _draw_info->text_antialias = ArgBoolean;
          break;
        }
      if (LocaleCompare("attenuate",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption("1.0"));
          break;
        }
      if (LocaleCompare("authenticate",option+1) == 0)
        {
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'b':
    {
      if (LocaleCompare("background",option+1) == 0)
        {
          /* FUTURE: both _image_info attribute & ImageOption in use!
             _image_info only used directly for generating new images.
             SyncImageSettings() used to set per-image attribute.

             FUTURE: if _image_info->background_color is not set then
             we should fall back to per-image background_color

             At this time -background will 'wipe out' the per-image
             background color!

             Better error handling of QueryColorCompliance() needed.
          */
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          (void) QueryColorCompliance(ArgOption(MogrifyBackgroundColor),AllCompliance,
             &_image_info->background_color,_exception);
          break;
        }
      if (LocaleCompare("bias",option+1) == 0)
        {
          /* FUTURE: bias OBSOLETED, replaced by Artifact "convolve:bias"
             as it is actually rarely used except in direct convolve operations
             Usage outside a direct convolve operation is actally non-sensible!

             SyncImageSettings() used to set per-image attribute.
          */
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,"convolve:bias",ArgOption(NULL));
          break;
        }
      if (LocaleCompare("black-point-compensation",option+1) == 0)
        {
          /* Used as a image chromaticity setting
             SyncImageSettings() used to set per-image attribute.
          */
          (void) SetImageOption(_image_info,option+1,ArgBooleanString);
          break;
        }
      if (LocaleCompare("blue-primary",option+1) == 0)
        {
          /* Image chromaticity X,Y  NB: Y=X if Y not defined
             Used by many coders including PNG
             SyncImageSettings() used to set per-image attribute.
          */
          arg1=ArgOption("0.0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("bordercolor",option+1) == 0)
        {
          /* FUTURE: both _image_info attribute & ImageOption in use!
             SyncImageSettings() used to set per-image attribute.
             Better error checking of QueryColorCompliance().
          */
          if (IfSetOption)
            {
              (void) SetImageOption(_image_info,option+1,arg1);
              (void) QueryColorCompliance(arg1,AllCompliance,
                  &_image_info->border_color,_exception);
              (void) QueryColorCompliance(arg1,AllCompliance,
                  &_draw_info->border_color,_exception);
              break;
            }
          (void) DeleteImageOption(_image_info,option+1);
          (void) QueryColorCompliance(MogrifyBorderColor,AllCompliance,
            &_image_info->border_color,_exception);
          (void) QueryColorCompliance(MogrifyBorderColor,AllCompliance,
            &_draw_info->border_color,_exception);
          break;
        }
      if (LocaleCompare("box",option+1) == 0)
        {
          CLIWandWarnReplaced("-undercolor");
          CLISettingOptionInfo(cli_wand,"-undercolor",arg1, arg2);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'c':
    {
      if (LocaleCompare("cache",option+1) == 0)
        {
          MagickSizeType
            limit;

          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          limit=MagickResourceInfinity;
          if (LocaleCompare("unlimited",arg1) != 0)
            limit=(MagickSizeType) SiPrefixToDoubleInterval(arg1,100.0);
          (void) SetMagickResourceLimit(MemoryResource,limit);
          (void) SetMagickResourceLimit(MapResource,2*limit);
          break;
        }
      if (LocaleCompare("caption",option+1) == 0)
        {
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("colorspace",option+1) == 0)
        {
          /* Setting used for new images via AquireImage()
             But also used as a SimpleImageOperator
             Undefined colorspace means don't modify images on
             read or as a operation */
          parse=ParseCommandOption(MagickColorspaceOptions,MagickFalse,
             ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedColorspace",option,
              arg1);
          _image_info->colorspace=(ColorspaceType) parse;
          break;
        }
      if (LocaleCompare("comment",option+1) == 0)
        {
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("compose",option+1) == 0)
        {
          /* FUTURE: _image_info should be used,
             SyncImageSettings() used to set per-image attribute. - REMOVE

             This setting should NOT be used to set image 'compose'
             "-layer" operators shoud use _image_info if defined otherwise
             they should use a per-image compose setting.
          */
          parse = ParseCommandOption(MagickComposeOptions,MagickFalse,
                          ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedComposeOperator",
                                      option,arg1);
          _image_info->compose=(CompositeOperator) parse;
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("compress",option+1) == 0)
        {
          /* FUTURE: What should be used?  _image_info  or ImageOption ???
             The former is more efficent, but Crisy prefers the latter!
             SyncImageSettings() used to set per-image attribute.

             The coders appears to use _image_info, not Image_Option
             however the image attribute (for save) is set from the
             ImageOption!

             Note that "undefined" is a different setting to "none".
          */
          parse = ParseCommandOption(MagickCompressOptions,MagickFalse,
                     ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedImageCompression",
                                      option,arg1);
          _image_info->compression=(CompressionType) parse;
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'd':
    {
      if (LocaleCompare("debug",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          arg1=ArgOption("none");
          parse = ParseCommandOption(MagickLogEventOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedEventType",
                                      option,arg1);
          (void) SetLogEventMask(arg1);
          _image_info->debug=IsEventLogging();   /* extract logging*/
          cli_wand->wand.debug=IsEventLogging();
          break;
        }
      if (LocaleCompare("define",option+1) == 0)
        {
          if (LocaleNCompare(arg1,"registry:",9) == 0)
            {
              if (IfSetOption)
                (void) DefineImageRegistry(StringRegistryType,arg1+9,_exception);
              else
                (void) DeleteImageRegistry(arg1+9);
              break;
            }
          /* DefineImageOption() equals SetImageOption() but with '=' */
          if (IfSetOption)
            (void) DefineImageOption(_image_info,arg1);
          else if (DeleteImageOption(_image_info,arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"NoSuchOption",option,arg1);
          break;
        }
      if (LocaleCompare("delay",option+1) == 0)
        {
          /* Only used for new images via AcquireImage()
             FUTURE: Option should also be used for "-morph" (color morphing)
          */
          arg1=ArgOption("0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("density",option+1) == 0)
        {
          /* FUTURE: strings used in _image_info attr and _draw_info!
             Basically as density can be in a XxY form!

             SyncImageSettings() used to set per-image attribute.
          */
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          (void) CloneString(&_image_info->density,ArgOption(NULL));
          (void) CloneString(&_draw_info->density,_image_info->density);
          break;
        }
      if (LocaleCompare("depth",option+1) == 0)
        {
          /* This is also a SimpleImageOperator! for 8->16 vaule trunc !!!!
             SyncImageSettings() used to set per-image attribute.
          */
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          _image_info->depth=IfSetOption?StringToUnsignedLong(arg1)
                                       :MAGICKCORE_QUANTUM_DEPTH;
          break;
        }
      if (LocaleCompare("direction",option+1) == 0)
        {
          /* Image Option is only used to set _draw_info */
          arg1=ArgOption("undefined");
          parse = ParseCommandOption(MagickDirectionOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedDirectionType",
                                      option,arg1);
          _draw_info->direction=(DirectionType) parse;
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("display",option+1) == 0)
        {
          (void) CloneString(&_image_info->server_name,ArgOption(NULL));
          (void) CloneString(&_draw_info->server_name,_image_info->server_name);
          break;
        }
      if (LocaleCompare("dispose",option+1) == 0)
        {
          /* only used in setting new images */
          arg1=ArgOption("undefined");
          parse = ParseCommandOption(MagickDisposeOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedDisposeMethod",
                                      option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption("undefined"));
          break;
        }
      if (LocaleCompare("dissimilarity-threshold",option+1) == 0)
        {
          /* FUTURE: this is only used by CompareImages() which is used
             only by the "compare" CLI program at this time.  */
          arg1=ArgOption(DEFAULT_DISSIMILARITY_THRESHOLD);
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("dither",option+1) == 0)
        {
          /* _image_info attr (on/off), _quantize_info attr (on/off)
             but also ImageInfo and _quantize_info method!
             FUTURE: merge the duality of the dithering options
          */
          _image_info->dither = ArgBoolean;
          (void) SetImageOption(_image_info,option+1,ArgOption("none"));
          _quantize_info->dither_method=(DitherMethod) ParseCommandOption(
             MagickDitherOptions,MagickFalse,ArgOption("none"));
          if (_quantize_info->dither_method == NoDitherMethod)
            _image_info->dither = MagickFalse;
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'e':
    {
      if (LocaleCompare("encoding",option+1) == 0)
        {
          (void) CloneString(&_draw_info->encoding,ArgOption("undefined"));
          (void) SetImageOption(_image_info,option+1,_draw_info->encoding);
          break;
        }
      if (LocaleCompare("endian",option+1) == 0)
        {
          /* Both _image_info attr and ImageInfo */
          arg1 = ArgOption("undefined");
          parse = ParseCommandOption(MagickEndianOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedEndianType",
                                      option,arg1);
          /* FUTURE: check alloc/free of endian string!  - remove? */
          _image_info->endian=(EndianType) (*arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("extract",option+1) == 0)
        {
          (void) CloneString(&_image_info->extract,ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'f':
    {
      if (LocaleCompare("family",option+1) == 0)
        {
          (void) CloneString(&_draw_info->family,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("features",option+1) == 0)
        {
          (void) SetImageOption(_image_info,"identify:features",
            ArgBooleanString);
          if (IfSetOption)
            (void) SetImageArtifact(_image,"verbose","true");
          break;
        }
      if (LocaleCompare("fill",option+1) == 0)
        {
          /* Set "fill" OR "fill-pattern" in _draw_info
             The original fill color is preserved if a fill-pattern is given.
             That way it does not effect other operations that directly using
             the fill color and, can be retored using "+tile".
          */
          MagickBooleanType
            status;

          ExceptionInfo
            *sans;

          PixelInfo
            color;

          arg1 = ArgOption("none");  /* +fill turns it off! */
          (void) SetImageOption(_image_info,option+1,arg1);
          if (_draw_info->fill_pattern != (Image *) NULL)
            _draw_info->fill_pattern=DestroyImage(_draw_info->fill_pattern);

          /* is it a color or a image? -- ignore exceptions */
          sans=AcquireExceptionInfo();
          status=QueryColorCompliance(arg1,AllCompliance,&color,sans);
          sans=DestroyExceptionInfo(sans);

          if (status == MagickFalse)
            _draw_info->fill_pattern=GetImageCache(_image_info,arg1,_exception);
          else
            _draw_info->fill=color;
          break;
        }
      if (LocaleCompare("filter",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          arg1 = ArgOption("undefined");
          parse = ParseCommandOption(MagickFilterOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedImageFilter",
                                      option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("font",option+1) == 0)
        {
          (void) CloneString(&_draw_info->font,ArgOption(NULL));
          (void) CloneString(&_image_info->font,_draw_info->font);
          break;
        }
      if (LocaleCompare("format",option+1) == 0)
        {
          /* FUTURE: why the ping test, you could set ping after this! */
          /*
          register const char
            *q;

          for (q=strchr(arg1,'%'); q != (char *) NULL; q=strchr(q+1,'%'))
            if (strchr("Agkrz@[#",*(q+1)) != (char *) NULL)
              _image_info->ping=MagickFalse;
          */
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("fuzz",option+1) == 0)
        {
          /* Option used to set image fuzz! unless blank canvas (from color)
             Image attribute used for color compare operations
             SyncImageSettings() used to set per-image attribute.

             FUTURE: Can't find anything else using _image_info->fuzz directly!
                     convert structure attribute to 'option' string
          */
          arg1=ArgOption("0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          _image_info->fuzz=StringToDoubleInterval(arg1,(double)
                QuantumRange+1.0);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'g':
    {
      if (LocaleCompare("gravity",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          arg1 = ArgOption("none");
          parse = ParseCommandOption(MagickGravityOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedGravityType",
                                      option,arg1);
          _draw_info->gravity=(GravityType) parse;
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("green-primary",option+1) == 0)
        {
          /* Image chromaticity X,Y  NB: Y=X if Y not defined
             SyncImageSettings() used to set per-image attribute.
             Used directly by many coders
          */
          arg1=ArgOption("0.0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'h':
    {
      if (LocaleCompare("highlight-color",option+1) == 0)
        {
          /* FUTURE: this is only used by CompareImages() which is used
             only by the "compare" CLI program at this time.  */
          (void) SetImageOption(_image_info,"compare:highlight-color",
            ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'i':
    {
      if (LocaleCompare("intensity",option+1) == 0)
        {
          arg1 = ArgOption("undefined");
          parse = ParseCommandOption(MagickPixelIntensityOptions,MagickFalse,
            arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedIntensityType",
              option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("intent",option+1) == 0)
        {
          /* Only used by coders: MIFF, MPC, BMP, PNG
             and for image profile call to AcquireTransformThreadSet()
             SyncImageSettings() used to set per-image attribute.
          */
          arg1 = ArgOption("undefined");
          parse = ParseCommandOption(MagickIntentOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedIntentType",
                                      option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("interlace",option+1) == 0)
        {
          /* _image_info is directly used by coders (so why an image setting?)
             SyncImageSettings() used to set per-image attribute.
          */
          arg1 = ArgOption("undefined");
          parse = ParseCommandOption(MagickInterlaceOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedInterlaceType",
                                      option,arg1);
          _image_info->interlace=(InterlaceType) parse;
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("interline-spacing",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1, ArgOption(NULL));
          _draw_info->interline_spacing=StringToDouble(ArgOption("0"),
               (char **) NULL);
          break;
        }
      if (LocaleCompare("interpolate",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          arg1 = ArgOption("undefined");
          parse = ParseCommandOption(MagickInterpolateOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedInterpolateMethod",
                                      option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("interword-spacing",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1, ArgOption(NULL));
          _draw_info->interword_spacing=StringToDouble(ArgOption("0"),(char **) NULL);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'k':
    {
      if (LocaleCompare("kerning",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          _draw_info->kerning=StringToDouble(ArgOption("0"),(char **) NULL);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'l':
    {
      if (LocaleCompare("label",option+1) == 0)
        {
          /* only used for new images - not in SyncImageOptions() */
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("limit",option+1) == 0)
        {
          MagickSizeType
            limit;

          limit=MagickResourceInfinity;
          parse= ParseCommandOption(MagickResourceOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedResourceType",
                option,arg1);
          if (LocaleCompare("unlimited",arg2) != 0)
            limit=(MagickSizeType) SiPrefixToDoubleInterval(arg2,100.0);
          (void) SetMagickResourceLimit((ResourceType)parse,limit);
          break;
        }
      if (LocaleCompare("log",option+1) == 0)
        {
          if (IfSetOption) {
            if ((strchr(arg1,'%') == (char *) NULL))
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            (void) SetLogFormat(arg1);
          }
          break;
        }
      if (LocaleCompare("lowlight-color",option+1) == 0)
        {
          /* FUTURE: this is only used by CompareImages() which is used
             only by the "compare" CLI program at this time.  */
          (void) SetImageOption(_image_info,"compare:lowlight-color",
            ArgOption(NULL));
          break;
        }
      if (LocaleCompare("loop",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          arg1=ArgOption("0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'm':
    {
      if (LocaleCompare("mattecolor",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          (void) QueryColorCompliance(ArgOption(MogrifyAlphaColor),
            AllCompliance,&_image_info->matte_color,_exception);
          break;
        }
      if (LocaleCompare("metric",option+1) == 0)
        {
          /* FUTURE: this is only used by CompareImages() which is used
             only by the "compare" CLI program at this time.  */
          parse=ParseCommandOption(MagickMetricOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedMetricType",
                option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("moments",option+1) == 0)
        {
          (void) SetImageOption(_image_info,"identify:moments",
            ArgBooleanString);
          if (IfSetOption)
            (void) SetImageArtifact(_image,"verbose","true");
          break;
        }
      if (LocaleCompare("monitor",option+1) == 0)
        {
          (void) SetImageInfoProgressMonitor(_image_info, IfSetOption?
                MonitorProgress: (MagickProgressMonitor) NULL, (void *) NULL);
          break;
        }
      if (LocaleCompare("monochrome",option+1) == 0)
        {
          /* Setting (used by some input coders!) -- why?
             Warning: This is also Special '-type' SimpleOperator
          */
          _image_info->monochrome= ArgBoolean;
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'o':
    {
      if (LocaleCompare("orient",option+1) == 0)
        {
          /* Is not used when defining for new images.
             This makes it more of a 'operation' than a setting
             FUTURE: make set meta-data operator instead.
             SyncImageSettings() used to set per-image attribute.
          */
          parse=ParseCommandOption(MagickOrientationOptions,MagickFalse,
               ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedImageOrientation",
                                      option,arg1);
          _image_info->orientation=(OrientationType)parse;
          (void) SetImageOption(_image_info,option+1, ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'p':
    {
      if (LocaleCompare("page",option+1) == 0)
        {
          /* Only used for new images and image generators.
             SyncImageSettings() used to set per-image attribute. ?????
             That last is WRONG!!!!
             FUTURE: adjust named 'page' sizes according density
          */
          char
            *canonical_page,
            page[MagickPathExtent];

          const char
            *image_option;

          MagickStatusType
            flags;

          RectangleInfo
            geometry;

          if (!IfSetOption)
            {
              (void) DeleteImageOption(_image_info,option+1);
              (void) CloneString(&_image_info->page,(char *) NULL);
              break;
            }
          (void) memset(&geometry,0,sizeof(geometry));
          image_option=GetImageOption(_image_info,"page");
          if (image_option != (const char *) NULL)
            flags=ParseAbsoluteGeometry(image_option,&geometry);
          canonical_page=GetPageGeometry(arg1);
          flags=ParseAbsoluteGeometry(canonical_page,&geometry);
          canonical_page=DestroyString(canonical_page);
          (void) FormatLocaleString(page,MagickPathExtent,"%lux%lu",
            (unsigned long) geometry.width,(unsigned long) geometry.height);
          if (((flags & XValue) != 0) || ((flags & YValue) != 0))
            (void) FormatLocaleString(page,MagickPathExtent,"%lux%lu%+ld%+ld",
              (unsigned long) geometry.width,(unsigned long) geometry.height,
              (long) geometry.x,(long) geometry.y);
          (void) SetImageOption(_image_info,option+1,page);
          (void) CloneString(&_image_info->page,page);
          break;
        }
      if (LocaleCompare("ping",option+1) == 0)
        {
          _image_info->ping = ArgBoolean;
          break;
        }
      if (LocaleCompare("pointsize",option+1) == 0)
        {
          if (IfSetOption) {
            if (IsGeometry(arg1) == MagickFalse)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            _image_info->pointsize =
            _draw_info->pointsize =
              StringToDouble(arg1,(char **) NULL);
          }
          else {
            _image_info->pointsize=0.0; /* unset pointsize */
            _draw_info->pointsize=12.0;
          }
          break;
        }
      if (LocaleCompare("precision",option+1) == 0)
        {
          arg1=ArgOption("-1");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetMagickPrecision(StringToInteger(arg1));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'q':
    {
      if (LocaleCompare("quality",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          _image_info->quality= IfSetOption ? StringToUnsignedLong(arg1)
                                            : UNDEFINED_COMPRESSION_QUALITY;
          (void) SetImageOption(_image_info,option+1,ArgOption("0"));
          break;
        }
      if (LocaleCompare("quantize",option+1) == 0)
        {
          /* Just a set direct in _quantize_info */
          arg1=ArgOption("undefined");
          parse=ParseCommandOption(MagickColorspaceOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedColorspace",
                 option,arg1);
          _quantize_info->colorspace=(ColorspaceType)parse;
          break;
        }
      if (LocaleCompare("quiet",option+1) == 0)
        {
          /* FUTURE: if two -quiet is performed you can not do +quiet!
             This needs to be checked over thoughly.
          */
          static WarningHandler
            warning_handler = (WarningHandler) NULL;

          WarningHandler
            tmp = SetWarningHandler((WarningHandler) NULL);

          if ( tmp != (WarningHandler) NULL)
            warning_handler = tmp; /* remember the old handler */
          if (!IfSetOption)        /* set the old handler */
            warning_handler=SetWarningHandler(warning_handler);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'r':
    {
      if (LocaleCompare("red-primary",option+1) == 0)
        {
          /* Image chromaticity X,Y  NB: Y=X if Y not defined
             Used by many coders
             SyncImageSettings() used to set per-image attribute.
          */
          arg1=ArgOption("0.0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("regard-warnings",option+1) == 0)
        /* FUTURE: to be replaced by a 'fatal-level' type setting */
        break;
      if (LocaleCompare("render",option+1) == 0)
        {
          /* _draw_info only setting */
          _draw_info->render= ArgBooleanNot;
          break;
        }
      if (LocaleCompare("respect-parenthesis",option+1) == 0)
        {
          /* link image and setting stacks - option is itself saved on stack! */
          (void) SetImageOption(_image_info,option+1,ArgBooleanString);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 's':
    {
      if (LocaleCompare("sampling-factor",option+1) == 0)
        {
          /* FUTURE: should be converted to jpeg:sampling_factor */
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) CloneString(&_image_info->sampling_factor,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("scene",option+1) == 0)
        {
          /* SyncImageSettings() used to set this as a per-image attribute.
             What ??? Why ????
          */
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          _image_info->scene=StringToUnsignedLong(ArgOption("0"));
          break;
        }
      if (LocaleCompare("seed",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          SetRandomSecretKey(
               IfSetOption ? (unsigned long) StringToUnsignedLong(arg1)
                           : (unsigned long) time((time_t *) NULL));
          break;
        }
      if (LocaleCompare("size",option+1) == 0)
        {
          /* FUTURE: string in _image_info -- convert to Option ???
             Look at the special handling for "size" in SetImageOption()
           */
          (void) CloneString(&_image_info->size,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("stretch",option+1) == 0)
        {
          arg1=ArgOption("undefined");
          parse = ParseCommandOption(MagickStretchOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedStretchType",
                 option,arg1);
          _draw_info->stretch=(StretchType) parse;
          break;
        }
      if (LocaleCompare("stroke",option+1) == 0)
        {
          /* set stroke color OR stroke-pattern
             UPDATE: ensure stroke color is not destroyed is a pattern
             is given. Just in case the color is also used for other purposes.
           */
          MagickBooleanType
            status;

          ExceptionInfo
            *sans;

          PixelInfo
            color;

          arg1 = ArgOption("none");  /* +fill turns it off! */
          (void) SetImageOption(_image_info,option+1,arg1);
          if (_draw_info->stroke_pattern != (Image *) NULL)
            _draw_info->stroke_pattern=DestroyImage(_draw_info->stroke_pattern);

          /* is it a color or a image? -- ignore exceptions */
          sans=AcquireExceptionInfo();
          status=QueryColorCompliance(arg1,AllCompliance,&color,sans);
          sans=DestroyExceptionInfo(sans);

          if (status == MagickFalse)
            _draw_info->stroke_pattern=GetImageCache(_image_info,arg1,_exception);
          else
            _draw_info->stroke=color;
          break;
        }
      if (LocaleCompare("strokewidth",option+1) == 0)
        {
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          _draw_info->stroke_width=StringToDouble(ArgOption("1.0"),
               (char **) NULL);
          break;
        }
      if (LocaleCompare("style",option+1) == 0)
        {
          arg1=ArgOption("undefined");
          parse = ParseCommandOption(MagickStyleOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedStyleType",
                 option,arg1);
          _draw_info->style=(StyleType) parse;
          break;
        }
#if 0
      if (LocaleCompare("subimage-search",option+1) == 0)
        {
        /* FUTURE: this is only used by CompareImages() which is used
            only by the "compare" CLI program at this time.  */
          (void) SetImageOption(_image_info,option+1,ArgBooleanString);
          break;
        }
#endif
      if (LocaleCompare("synchronize",option+1) == 0)
        {
          /* FUTURE: syncronize to storage - but what does that mean? */
          _image_info->synchronize = ArgBoolean;
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 't':
    {
      if (LocaleCompare("taint",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          (void) SetImageOption(_image_info,option+1,ArgBooleanString);
          break;
        }
      if (LocaleCompare("texture",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          /* FUTURE: move _image_info string to option splay-tree
             Other than "montage" what uses "texture" ????
          */
          (void) CloneString(&_image_info->texture,ArgOption(NULL));
          break;
        }
      if (LocaleCompare("tile",option+1) == 0)
        {
          /* Note: arguments do not have percent escapes expanded */
          _draw_info->fill_pattern=IfSetOption
                                 ?GetImageCache(_image_info,arg1,_exception)
                                 :DestroyImage(_draw_info->fill_pattern);
          break;
        }
      if (LocaleCompare("tile-offset",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. ??? */
          arg1=ArgOption("0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("transparent-color",option+1) == 0)
        {
          /* FUTURE: both _image_info attribute & ImageOption in use!
             _image_info only used for generating new images.
             SyncImageSettings() used to set per-image attribute.

             Note that +transparent-color, means fall-back to image
             attribute so ImageOption is deleted, not set to a default.
          */
          if (IfSetOption && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          (void) QueryColorCompliance(ArgOption("none"),AllCompliance,
              &_image_info->transparent_color,_exception);
          break;
        }
      if (LocaleCompare("treedepth",option+1) == 0)
        {
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          _quantize_info->tree_depth=StringToUnsignedLong(ArgOption("0"));
          break;
        }
      if (LocaleCompare("type",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute. */
          parse=ParseCommandOption(MagickTypeOptions,MagickFalse,
               ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedImageType",
                 option,arg1);
          _image_info->type=(ImageType) parse;
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'u':
    {
      if (LocaleCompare("undercolor",option+1) == 0)
        {
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          (void) QueryColorCompliance(ArgOption("none"),AllCompliance,
               &_draw_info->undercolor,_exception);
          break;
        }
      if (LocaleCompare("units",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute.
             Should this effect _draw_info X and Y resolution?
             FUTURE: this probably should be part of the density setting
          */
          parse=ParseCommandOption(MagickResolutionOptions,MagickFalse,
               ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedUnitsType",
                 option,arg1);
          _image_info->units=(ResolutionType) parse;
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'v':
    {
      if (LocaleCompare("verbose",option+1) == 0)
        {
          /* FUTURE: Remember all options become image artifacts
             _image_info->verbose is only used by coders.
          */
          (void) SetImageOption(_image_info,option+1,ArgBooleanString);
          _image_info->verbose= ArgBoolean;
          _image_info->ping=MagickFalse; /* verbose can't be a ping */
          break;
        }
      if (LocaleCompare("virtual-pixel",option+1) == 0)
        {
          /* SyncImageSettings() used to set per-image attribute.
             This is VERY deep in the image caching structure.
          */
          parse=ParseCommandOption(MagickVirtualPixelOptions,MagickFalse,
               ArgOption("undefined"));
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedVirtualPixelMethod",
                 option,arg1);
          (void) SetImageOption(_image_info,option+1,ArgOption(NULL));
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'w':
    {
      if (LocaleCompare("weight",option+1) == 0)
        {
          ssize_t
            weight;

          weight=ParseCommandOption(MagickWeightOptions,MagickFalse,arg1);
          if (weight == -1)
            weight=(ssize_t) StringToUnsignedLong(arg1);
          _draw_info->weight=(size_t) weight;
          break;
        }
      if (LocaleCompare("white-point",option+1) == 0)
        {
          /* Used as a image chromaticity setting
             SyncImageSettings() used to set per-image attribute.
          */
          arg1=ArgOption("0.0");
          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    default:
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
  }

  /* clean up percent escape interpreted strings */
  if ((arg1 && arg1n) && (arg1 != arg1n ))
    arg1=DestroyString((char *) arg1);
  if ((arg2 && arg2n) && (arg2 != arg2n ))
    arg2=DestroyString((char *) arg2);

#undef _image_info
#undef _exception
#undef _draw_info
#undef _quantize_info
#undef IfSetOption
#undef ArgBoolean
#undef ArgBooleanNot
#undef ArgBooleanString
#undef ArgOption

  return;
}