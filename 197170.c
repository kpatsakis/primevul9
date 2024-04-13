WandExport MagickBooleanType MogrifyImageInfo(ImageInfo *image_info,
  const int argc,const char **argv,ExceptionInfo *exception)
{
  const char
    *option;

  GeometryInfo
    geometry_info;

  ssize_t
    count;

  register ssize_t
    i;

  /*
    Initialize method variables.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (argc < 0)
    return(MagickTrue);
  /*
    Set the image settings.
  */
  for (i=0; i < (ssize_t) argc; i++)
  {
    option=argv[i];
    if (IsCommandOption(option) == MagickFalse)
      continue;
    count=ParseCommandOption(MagickCommandOptions,MagickFalse,option);
    count=MagickMax(count,0L);
    if ((i+count) >= (ssize_t) argc)
      break;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adjoin",option+1) == 0)
          {
            image_info->adjoin=(*option == '-') ? MagickTrue : MagickFalse;
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            image_info->antialias=(*option == '-') ? MagickTrue : MagickFalse;
            break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            if (*option == '+')
              (void) DeleteImageOption(image_info,option+1);
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageOption(image_info,option+1);
                (void) QueryColorCompliance(MogrifyBackgroundColor,
                  AllCompliance,&image_info->background_color,exception);
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &image_info->background_color,exception);
            break;
          }
        if (LocaleCompare("bias",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,"convolve:bias","0.0");
                break;
              }
            (void) SetImageOption(image_info,"convolve:bias",argv[i+1]);
            break;
          }
        if (LocaleCompare("black-point-compensation",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"false");
                break;
              }
            (void) SetImageOption(image_info,option+1,"true");
            break;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"0.0");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageOption(image_info,option+1);
                (void) QueryColorCompliance(MogrifyBorderColor,AllCompliance,
                  &image_info->border_color,exception);
                break;
              }
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &image_info->border_color,exception);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,"undercolor","none");
                break;
              }
            (void) SetImageOption(image_info,"undercolor",argv[i+1]);
            break;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            MagickSizeType
              limit;

            limit=MagickResourceInfinity;
            if (LocaleCompare("unlimited",argv[i+1]) != 0)
              limit=(MagickSizeType) SiPrefixToDoubleInterval(argv[i+1],
                100.0);
            (void) SetMagickResourceLimit(MemoryResource,limit);
            (void) SetMagickResourceLimit(MapResource,2*limit);
            break;
          }
        if (LocaleCompare("caption",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageOption(image_info,option+1);
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->colorspace=UndefinedColorspace;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->colorspace=(ColorspaceType) ParseCommandOption(
              MagickColorspaceOptions,MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageOption(image_info,option+1);
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->compression=UndefinedCompression;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->compression=(CompressionType) ParseCommandOption(
              MagickCompressOptions,MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '+')
              (void) SetLogEventMask("none");
            else
              (void) SetLogEventMask(argv[i+1]);
            image_info->debug=IsEventLogging();
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            if (*option == '+')
              {
                if (LocaleNCompare(argv[i+1],"registry:",9) == 0)
                  (void) DeleteImageRegistry(argv[i+1]+9);
                else
                  (void) DeleteImageOption(image_info,argv[i+1]);
                break;
              }
            if (LocaleNCompare(argv[i+1],"registry:",9) == 0)
              {
                (void) DefineImageRegistry(StringRegistryType,argv[i+1]+9,
                  exception);
                break;
              }
            (void) DefineImageOption(image_info,argv[i+1]);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"0");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            /*
              Set image density.
            */
            if (*option == '+')
              {
                if (image_info->density != (char *) NULL)
                  image_info->density=DestroyString(image_info->density);
                (void) SetImageOption(image_info,option+1,"72");
                break;
              }
            (void) CloneString(&image_info->density,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->depth=MAGICKCORE_QUANTUM_DEPTH;
                break;
              }
            image_info->depth=StringToUnsignedLong(argv[i+1]);
            break;
          }
        if (LocaleCompare("direction",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            if (*option == '+')
              {
                if (image_info->server_name != (char *) NULL)
                  image_info->server_name=DestroyString(
                    image_info->server_name);
                break;
              }
            (void) CloneString(&image_info->server_name,argv[i+1]);
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->dither=MagickFalse;
                (void) SetImageOption(image_info,option+1,"none");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            image_info->dither=MagickTrue;
            break;
          }
        break;
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->endian=UndefinedEndian;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->endian=(EndianType) ParseCommandOption(
              MagickEndianOptions,MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("extract",option+1) == 0)
          {
            /*
              Set image extract geometry.
            */
            if (*option == '+')
              {
                if (image_info->extract != (char *) NULL)
                  image_info->extract=DestroyString(image_info->extract);
                break;
              }
            (void) CloneString(&image_info->extract,argv[i+1]);
            break;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("family",option+1) == 0)
          {
            if (*option != '+')
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("fill",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"none");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("filter",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("font",option+1) == 0)
          {
            if (*option == '+')
              {
                if (image_info->font != (char *) NULL)
                  image_info->font=DestroyString(image_info->font);
                break;
              }
            (void) CloneString(&image_info->font,argv[i+1]);
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            register const char
              *q;

            for (q=strchr(argv[i+1],'%'); q != (char *) NULL; q=strchr(q+1,'%'))
              if (strchr("Agkrz@[#",*(q+1)) != (char *) NULL)
                image_info->ping=MagickFalse;
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->fuzz=0.0;
                (void) SetImageOption(image_info,option+1,"0");
                break;
              }
            image_info->fuzz=StringToDoubleInterval(argv[i+1],(double)
              QuantumRange+1.0);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gravity",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"0.0");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'i':
      {
        if (LocaleCompare("intensity",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("intent",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->interlace=UndefinedInterlace;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->interlace=(InterlaceType) ParseCommandOption(
              MagickInterlaceOptions,MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("interline-spacing",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("interpolate",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("interword-spacing",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'k':
      {
        if (LocaleCompare("kerning",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) DeleteImageOption(image_info,option+1);
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            MagickSizeType
              limit;

            ResourceType
              type;

            if (*option == '+')
              break;
            type=(ResourceType) ParseCommandOption(MagickResourceOptions,
              MagickFalse,argv[i+1]);
            limit=MagickResourceInfinity;
            if (LocaleCompare("unlimited",argv[i+2]) != 0)
              limit=(MagickSizeType) SiPrefixToDoubleInterval(argv[i+2],100.0);
            (void) SetMagickResourceLimit(type,limit);
            break;
          }
        if (LocaleCompare("list",option+1) == 0)
          {
            ssize_t
              list;

            /*
              Display configuration list.
            */
            list=ParseCommandOption(MagickListOptions,MagickFalse,argv[i+1]);
            switch (list)
            {
              case MagickCoderOptions:
              {
                (void) ListCoderInfo((FILE *) NULL,exception);
                break;
              }
              case MagickColorOptions:
              {
                (void) ListColorInfo((FILE *) NULL,exception);
                break;
              }
              case MagickConfigureOptions:
              {
                (void) ListConfigureInfo((FILE *) NULL,exception);
                break;
              }
              case MagickDelegateOptions:
              {
                (void) ListDelegateInfo((FILE *) NULL,exception);
                break;
              }
              case MagickFontOptions:
              {
                (void) ListTypeInfo((FILE *) NULL,exception);
                break;
              }
              case MagickFormatOptions:
              {
                (void) ListMagickInfo((FILE *) NULL,exception);
                break;
              }
              case MagickLocaleOptions:
              {
                (void) ListLocaleInfo((FILE *) NULL,exception);
                break;
              }
              case MagickLogOptions:
              {
                (void) ListLogInfo((FILE *) NULL,exception);
                break;
              }
              case MagickMagicOptions:
              {
                (void) ListMagicInfo((FILE *) NULL,exception);
                break;
              }
              case MagickMimeOptions:
              {
                (void) ListMimeInfo((FILE *) NULL,exception);
                break;
              }
              case MagickModuleOptions:
              {
                (void) ListModuleInfo((FILE *) NULL,exception);
                break;
              }
              case MagickPolicyOptions:
              {
                (void) ListPolicyInfo((FILE *) NULL,exception);
                break;
              }
              case MagickResourceOptions:
              {
                (void) ListMagickResourceInfo((FILE *) NULL,exception);
                break;
              }
              case MagickThresholdOptions:
              {
                (void) ListThresholdMaps((FILE *) NULL,exception);
                break;
              }
              default:
              {
                (void) ListCommandOptions((FILE *) NULL,(CommandOption) list,
                  exception);
                break;
              }
            }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '+')
              break;
            (void) SetLogFormat(argv[i+1]);
            break;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"0");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("matte",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"false");
                break;
              }
            (void) SetImageOption(image_info,option+1,"true");
            break;
          }
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,argv[i+1]);
                (void) QueryColorCompliance(MogrifyAlphaColor,AllCompliance,
                  &image_info->matte_color,exception);
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &image_info->matte_color,exception);
            break;
          }
        if (LocaleCompare("metric",option+1) == 0)
          {
            if (*option == '+')
              (void) DeleteImageOption(image_info,option+1);
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            (void) SetImageInfoProgressMonitor(image_info,MonitorProgress,
              (void *) NULL);
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-') ? MagickTrue : MagickFalse;
            break;
          }
        break;
      }
      case 'o':
      {
        if (LocaleCompare("orient",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->orientation=UndefinedOrientation;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->orientation=(OrientationType) ParseCommandOption(
              MagickOrientationOptions,MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            char
              *canonical_page,
              page[MagickPathExtent];

            const char
              *image_option;

            MagickStatusType
              flags;

            RectangleInfo
              geometry;

            if (*option == '+')
              {
                (void) DeleteImageOption(image_info,option+1);
                (void) CloneString(&image_info->page,(char *) NULL);
                break;
              }
            (void) memset(&geometry,0,sizeof(geometry));
            image_option=GetImageOption(image_info,"page");
            if (image_option != (const char *) NULL)
              flags=ParseAbsoluteGeometry(image_option,&geometry);
            canonical_page=GetPageGeometry(argv[i+1]);
            flags=ParseAbsoluteGeometry(canonical_page,&geometry);
            canonical_page=DestroyString(canonical_page);
            (void) FormatLocaleString(page,MagickPathExtent,"%lux%lu",
              (unsigned long) geometry.width,(unsigned long) geometry.height);
            if (((flags & XValue) != 0) || ((flags & YValue) != 0))
              (void) FormatLocaleString(page,MagickPathExtent,"%lux%lu%+ld%+ld",
                (unsigned long) geometry.width,(unsigned long) geometry.height,
                (long) geometry.x,(long) geometry.y);
            (void) SetImageOption(image_info,option+1,page);
            (void) CloneString(&image_info->page,page);
            break;
          }
        if (LocaleCompare("ping",option+1) == 0)
          {
            image_info->ping=(*option == '-') ? MagickTrue : MagickFalse;
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            if (*option == '+')
              geometry_info.rho=0.0;
            else
              (void) ParseGeometry(argv[i+1],&geometry_info);
            image_info->pointsize=geometry_info.rho;
            break;
          }
        if (LocaleCompare("precision",option+1) == 0)
          {
            (void) SetMagickPrecision(StringToInteger(argv[i+1]));
            break;
          }
        break;
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            /*
              Set image compression quality.
            */
            if (*option == '+')
              {
                image_info->quality=UndefinedCompressionQuality;
                (void) SetImageOption(image_info,option+1,"0");
                break;
              }
            image_info->quality=StringToUnsignedLong(argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("quiet",option+1) == 0)
          {
            static WarningHandler
              warning_handler = (WarningHandler) NULL;

            if (*option == '+')
              {
                /*
                  Restore error or warning messages.
                */
                warning_handler=SetWarningHandler(warning_handler);
                break;
              }
            /*
              Suppress error or warning messages.
            */
            warning_handler=SetWarningHandler((WarningHandler) NULL);
            break;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"0.0");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            /*
              Set image sampling factor.
            */
            if (*option == '+')
              {
                if (image_info->sampling_factor != (char *) NULL)
                  image_info->sampling_factor=DestroyString(
                    image_info->sampling_factor);
                break;
              }
            (void) CloneString(&image_info->sampling_factor,argv[i+1]);
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            /*
              Set image scene.
            */
            if (*option == '+')
              {
                image_info->scene=0;
                (void) SetImageOption(image_info,option+1,"0");
                break;
              }
            image_info->scene=StringToUnsignedLong(argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("seed",option+1) == 0)
          {
            unsigned long
              seed;

            if (*option == '+')
              {
                seed=(unsigned long) time((time_t *) NULL);
                SetRandomSecretKey(seed);
                break;
              }
            seed=StringToUnsignedLong(argv[i+1]);
            SetRandomSecretKey(seed);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            if (*option == '+')
              {
                if (image_info->size != (char *) NULL)
                  image_info->size=DestroyString(image_info->size);
                break;
              }
            (void) CloneString(&image_info->size,argv[i+1]);
            break;
          }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"none");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '+')
              (void) SetImageOption(image_info,option+1,"0");
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("style",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"none");
                break;
              }
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("synchronize",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->synchronize=MagickFalse;
                break;
              }
            image_info->synchronize=MagickTrue;
            break;
          }
        break;
      }
      case 't':
      {
        if (LocaleCompare("taint",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageOption(image_info,option+1,"false");
                break;
              }
            (void) SetImageOption(image_info,option+1,"true");
            break;
          }
        if (LocaleCompare("texture",option+1) == 0)
          {
            if (*option == '+')
              {
                if (image_info->texture != (char *) NULL)
                  image_info->texture=DestroyString(image_info->texture);
                break;
              }
            (void) CloneString(&image_info->texture,argv[i+1]);
            break;
          }
        if (LocaleCompare("tile-offset",option+1) == 0)
          {
            if (*option == '+')
              (void) SetImageOption(image_info,option+1,"0");
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("transparent-color",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) QueryColorCompliance("none",AllCompliance,
                  &image_info->transparent_color,exception);
                (void) SetImageOption(image_info,option+1,"none");
                break;
              }
            (void) QueryColorCompliance(argv[i+1],AllCompliance,
              &image_info->transparent_color,exception);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->type=UndefinedType;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->type=(ImageType) ParseCommandOption(MagickTypeOptions,
              MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            if (*option == '+')
              (void) DeleteImageOption(image_info,option+1);
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->units=UndefinedResolution;
                (void) SetImageOption(image_info,option+1,"undefined");
                break;
              }
            image_info->units=(ResolutionType) ParseCommandOption(
              MagickResolutionOptions,MagickFalse,argv[i+1]);
            (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            if (*option == '+')
              {
                image_info->verbose=MagickFalse;
                break;
              }
            image_info->verbose=MagickTrue;
            image_info->ping=MagickFalse;
            break;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '+')
              (void) SetImageOption(image_info,option+1,"undefined");
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("weight",option+1) == 0)
          {
            if (*option == '+')
              (void) SetImageOption(image_info,option+1,"0");
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        if (LocaleCompare("white-point",option+1) == 0)
          {
            if (*option == '+')
              (void) SetImageOption(image_info,option+1,"0.0");
            else
              (void) SetImageOption(image_info,option+1,argv[i+1]);
            break;
          }
        break;
      }
      default:
        break;
    }
    i+=count;
  }
  return(MagickTrue);
}