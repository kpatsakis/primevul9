WandPrivate MagickBooleanType CLIListOperatorImages(MagickCLI *cli_wand,
  const char *option,const char *arg1n,const char *arg2n)
{
  const char    /* percent escaped versions of the args */
    *arg1,
    *arg2;

  Image
    *new_images;

  MagickStatusType
    status;

  ssize_t
    parse;

#define _image_info     (cli_wand->wand.image_info)
#define _images         (cli_wand->wand.images)
#define _exception      (cli_wand->wand.exception)
#define _draw_info      (cli_wand->draw_info)
#define _quantize_info  (cli_wand->quantize_info)
#define _process_flags  (cli_wand->process_flags)
#define _option_type    ((CommandOptionFlags) cli_wand->command->flags)
#define IfNormalOp      (*option=='-')
#define IfPlusOp        (*option!='-')
#define IsNormalOp      IfNormalOp ? MagickTrue : MagickFalse

  assert(cli_wand != (MagickCLI *) NULL);
  assert(cli_wand->signature == MagickWandSignature);
  assert(cli_wand->wand.signature == MagickWandSignature);
  assert(_images != (Image *) NULL);             /* _images must be present */

  if (cli_wand->wand.debug != MagickFalse)
    (void) CLILogEvent(cli_wand,CommandEvent,GetMagickModule(),
       "- List Operator: %s \"%s\" \"%s\"", option,
       arg1n == (const char *) NULL ? "null" : arg1n,
       arg2n == (const char *) NULL ? "null" : arg2n);

  arg1 = arg1n;
  arg2 = arg2n;

  /* Interpret Percent Escapes in Arguments - using first image */
  if ( (((_process_flags & ProcessInterpretProperities) != 0 )
        || ((_option_type & AlwaysInterpretArgsFlag) != 0)
       )  && ((_option_type & NeverInterpretArgsFlag) == 0) ) {
    /* Interpret Percent escapes in argument 1 */
    if (arg1n != (char *) NULL) {
      arg1=InterpretImageProperties(_image_info,_images,arg1n,_exception);
      if (arg1 == (char *) NULL) {
        CLIWandException(OptionWarning,"InterpretPropertyFailure",option);
        arg1=arg1n;  /* use the given argument as is */
      }
    }
    if (arg2n != (char *) NULL) {
      arg2=InterpretImageProperties(_image_info,_images,arg2n,_exception);
      if (arg2 == (char *) NULL) {
        CLIWandException(OptionWarning,"InterpretPropertyFailure",option);
        arg2=arg2n;  /* use the given argument as is */
      }
    }
  }
#undef _process_flags
#undef _option_type

  status=MagickTrue;
  new_images=NewImageList();

  switch (*(option+1))
  {
    case 'a':
    {
      if (LocaleCompare("append",option+1) == 0)
        {
          new_images=AppendImages(_images,IsNormalOp,_exception);
          break;
        }
      if (LocaleCompare("average",option+1) == 0)
        {
          CLIWandWarnReplaced("-evaluate-sequence Mean");
          (void) CLIListOperatorImages(cli_wand,"-evaluate-sequence","Mean",
            NULL);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'c':
    {
      if (LocaleCompare("channel-fx",option+1) == 0)
        {
          new_images=ChannelFxImage(_images,arg1,_exception);
          break;
        }
      if (LocaleCompare("clut",option+1) == 0)
        {
          Image
            *clut_image;

          /* FUTURE - make this a compose option, and thus can be used
             with layers compose or even compose last image over all other
             _images.
          */
          new_images=RemoveFirstImageFromList(&_images);
          clut_image=RemoveFirstImageFromList(&_images);
          /* FUTURE - produce Exception, rather than silent fail */
          if (clut_image == (Image *) NULL)
            {
              (void) ThrowMagickException(_exception,GetMagickModule(),
                OptionError,"ImageSequenceRequired","`%s'",option);
              new_images=DestroyImage(new_images);
              status=MagickFalse;
              break;
            }
          (void) ClutImage(new_images,clut_image,new_images->interpolate,
            _exception);
          clut_image=DestroyImage(clut_image);
          break;
        }
      if (LocaleCompare("coalesce",option+1) == 0)
        {
          new_images=CoalesceImages(_images,_exception);
          break;
        }
      if (LocaleCompare("combine",option+1) == 0)
        {
          parse=(ssize_t) _images->colorspace;
          if (_images->number_channels < GetImageListLength(_images))
            parse=sRGBColorspace;
          if ( IfPlusOp )
            parse=ParseCommandOption(MagickColorspaceOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedColorspace",option,
              arg1);
          new_images=CombineImages(_images,(ColorspaceType) parse,_exception);
          break;
        }
      if (LocaleCompare("compare",option+1) == 0)
        {
          double
            distortion;

          Image
            *image,
            *reconstruct_image;

          MetricType
            metric;

          /*
            Mathematically and visually annotate the difference between an
            image and its reconstruction.
          */
          image=RemoveFirstImageFromList(&_images);
          reconstruct_image=RemoveFirstImageFromList(&_images);
          /* FUTURE - produce Exception, rather than silent fail */
          if (reconstruct_image == (Image *) NULL)
            {
              (void) ThrowMagickException(_exception,GetMagickModule(),
                OptionError,"ImageSequenceRequired","`%s'",option);
              image=DestroyImage(image);
              status=MagickFalse;
              break;
            }
          metric=UndefinedErrorMetric;
          option=GetImageOption(_image_info,"metric");
          if (option != (const char *) NULL)
            metric=(MetricType) ParseCommandOption(MagickMetricOptions,
              MagickFalse,option);
          new_images=CompareImages(image,reconstruct_image,metric,&distortion,
            _exception);
          (void) distortion;
          reconstruct_image=DestroyImage(reconstruct_image);
          image=DestroyImage(image);
          break;
        }
      if (LocaleCompare("complex",option+1) == 0)
        {
          parse=ParseCommandOption(MagickComplexOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedEvaluateOperator",
              option,arg1);
          new_images=ComplexImages(_images,(ComplexOperator) parse,_exception);
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
            *source_image;

          RectangleInfo
            geometry;

          /* Compose value from "-compose" option only */
          value=GetImageOption(_image_info,"compose");
          if (value == (const char *) NULL)
            compose=OverCompositeOp;  /* use Over not source_image->compose */
          else
            compose=(CompositeOperator) ParseCommandOption(MagickComposeOptions,
              MagickFalse,value);

          /* Get "clip-to-self" expert setting (false is normal) */
          clip_to_self=GetCompositeClipToSelf(compose);
          value=GetImageOption(_image_info,"compose:clip-to-self");
          if (value != (const char *) NULL)
            clip_to_self=IsStringTrue(value);
          value=GetImageOption(_image_info,"compose:outside-overlay");
          if (value != (const char *) NULL)
            clip_to_self=IsStringFalse(value);  /* deprecated */

          new_images=RemoveFirstImageFromList(&_images);
          source_image=RemoveFirstImageFromList(&_images);
          if (source_image == (Image *) NULL)
            {
              (void) ThrowMagickException(_exception,GetMagickModule(),
                OptionError,"ImageSequenceRequired","`%s'",option);
              new_images=DestroyImage(new_images);
              status=MagickFalse;
              break;
            }

          /* FUTURE - this should not be here! - should be part of -geometry */
          if (source_image->geometry != (char *) NULL)
            {
              RectangleInfo
                resize_geometry;

              (void) ParseRegionGeometry(source_image,source_image->geometry,
                &resize_geometry,_exception);
              if ((source_image->columns != resize_geometry.width) ||
                  (source_image->rows != resize_geometry.height))
                {
                  Image
                    *resize_image;

                  resize_image=ResizeImage(source_image,resize_geometry.width,
                    resize_geometry.height,source_image->filter,_exception);
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
            new_images->gravity, &geometry);
          mask_image=RemoveFirstImageFromList(&_images);
          if (mask_image == (Image *) NULL)
            status&=CompositeImage(new_images,source_image,compose,clip_to_self,
              geometry.x,geometry.y,_exception);
          else
            {
              if ((compose == DisplaceCompositeOp) ||
                  (compose == DistortCompositeOp))
                {
                  status&=CompositeImage(source_image,mask_image,
                    CopyGreenCompositeOp,MagickTrue,0,0,_exception);
                  status&=CompositeImage(new_images,source_image,compose,
                    clip_to_self,geometry.x,geometry.y,_exception);
                }
              else
                {
                  Image
                    *clone_image;

                  clone_image=CloneImage(new_images,0,0,MagickTrue,_exception);
                  if (clone_image == (Image *) NULL)
                    break;
                  status&=CompositeImage(new_images,source_image,compose,
                    clip_to_self,geometry.x,geometry.y,_exception);
                  status&=CompositeImage(new_images,mask_image,
                    CopyAlphaCompositeOp,MagickTrue,0,0,_exception);
                  status&=CompositeImage(clone_image,new_images,OverCompositeOp,
                    clip_to_self,0,0,_exception);
                  new_images=DestroyImageList(new_images);
                  new_images=clone_image;
                }
              mask_image=DestroyImage(mask_image);
            }
          source_image=DestroyImage(source_image);
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
            if (IsGeometry(arg1) == MagickFalse)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            if (IsGeometry(arg2) == MagickFalse)
              CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
            (void) ParsePageGeometry(_images,arg2,&geometry,_exception);
            offset.x=geometry.x;
            offset.y=geometry.y;
            source_image=_images;
            if (source_image->next != (Image *) NULL)
              source_image=source_image->next;
            (void) ParsePageGeometry(source_image,arg1,&geometry,_exception);
            (void) CopyImagePixels(_images,source_image,&geometry,&offset,
              _exception);
            break;
          }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'd':
    {
      if (LocaleCompare("deconstruct",option+1) == 0)
        {
          CLIWandWarnReplaced("-layer CompareAny");
          (void) CLIListOperatorImages(cli_wand,"-layer","CompareAny",NULL);
          break;
        }
      if (LocaleCompare("delete",option+1) == 0)
        {
          if (IfNormalOp)
            DeleteImages(&_images,arg1,_exception);
          else
            DeleteImages(&_images,"-1",_exception);
          break;
        }
      if (LocaleCompare("duplicate",option+1) == 0)
        {
          if (IfNormalOp)
            {
              const char
                *p;

              size_t
                number_duplicates;

              if (IsGeometry(arg1) == MagickFalse)
                CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,
                      arg1);
              number_duplicates=(size_t) StringToLong(arg1);
              p=strchr(arg1,',');
              if (p == (const char *) NULL)
                new_images=DuplicateImages(_images,number_duplicates,"-1",
                  _exception);
              else
                new_images=DuplicateImages(_images,number_duplicates,p,
                  _exception);
            }
          else
            new_images=DuplicateImages(_images,1,"-1",_exception);
          AppendImageToList(&_images, new_images);
          new_images=(Image *) NULL;
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'e':
    {
      if (LocaleCompare("evaluate-sequence",option+1) == 0)
        {
          parse=ParseCommandOption(MagickEvaluateOptions,MagickFalse,arg1);
          if (parse < 0)
            CLIWandExceptArgBreak(OptionError,"UnrecognizedEvaluateOperator",
              option,arg1);
          new_images=EvaluateImages(_images,(MagickEvaluateOperator) parse,
            _exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'f':
    {
      if (LocaleCompare("fft",option+1) == 0)
        {
          new_images=ForwardFourierTransformImage(_images,IsNormalOp,
           _exception);
          break;
        }
      if (LocaleCompare("flatten",option+1) == 0)
        {
          /* REDIRECTED to use -layers flatten instead */
          (void) CLIListOperatorImages(cli_wand,"-layers",option+1,NULL);
          break;
        }
      if (LocaleCompare("fx",option+1) == 0)
        {
          new_images=FxImage(_images,arg1,_exception);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'h':
    {
      if (LocaleCompare("hald-clut",option+1) == 0)
        {
          /* FUTURE - make this a compose option (and thus layers compose )
             or perhaps compose last image over all other _images.
          */
          Image
            *hald_image;

          new_images=RemoveFirstImageFromList(&_images);
          hald_image=RemoveLastImageFromList(&_images);
          if (hald_image == (Image *) NULL)
            {
              (void) ThrowMagickException(_exception,GetMagickModule(),
                OptionError,"ImageSequenceRequired","`%s'",option);
              new_images=DestroyImage(new_images);
              status=MagickFalse;
              break;
            }
          (void) HaldClutImage(new_images,hald_image,_exception);
          hald_image=DestroyImage(hald_image);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'i':
    {
      if (LocaleCompare("ift",option+1) == 0)
        {
          Image
            *magnitude_image,
            *phase_image;

          magnitude_image=RemoveFirstImageFromList(&_images);
          phase_image=RemoveFirstImageFromList(&_images);
          if (phase_image == (Image *) NULL)
            {
              (void) ThrowMagickException(_exception,GetMagickModule(),
                OptionError,"ImageSequenceRequired","`%s'",option);
              magnitude_image=DestroyImage(magnitude_image);
              status=MagickFalse;
              break;
            }
          new_images=InverseFourierTransformImage(magnitude_image,phase_image,
            IsNormalOp,_exception);
          magnitude_image=DestroyImage(magnitude_image);
          phase_image=DestroyImage(phase_image);
          break;
        }
      if (LocaleCompare("insert",option+1) == 0)
        {
          Image
            *insert_image,
            *index_image;

          ssize_t
            index;

          if (IfNormalOp && (IsGeometry(arg1) == MagickFalse))
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          index=0;
          insert_image=RemoveLastImageFromList(&_images);
          if (IfNormalOp)
            index=(ssize_t) StringToLong(arg1);
          index_image=insert_image;
          if (index == 0)
            PrependImageToList(&_images,insert_image);
          else if (index == (ssize_t) GetImageListLength(_images))
            AppendImageToList(&_images,insert_image);
          else
            {
               index_image=GetImageFromList(_images,index-1);
               if (index_image == (Image *) NULL)
                 {
                   insert_image=DestroyImage(insert_image);
                   CLIWandExceptArgBreak(OptionError,"NoSuchImage",option,arg1);
                 }
              InsertImageInList(&index_image,insert_image);
            }
          _images=GetFirstImageInList(index_image);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'l':
    {
      if (LocaleCompare("layers",option+1) == 0)
        {
          parse=ParseCommandOption(MagickLayerOptions,MagickFalse,arg1);
          if ( parse < 0 )
            CLIWandExceptArgBreak(OptionError,"UnrecognizedLayerMethod",
                 option,arg1);
          switch ((LayerMethod) parse)
          {
            case CoalesceLayer:
            {
              new_images=CoalesceImages(_images,_exception);
              break;
            }
            case CompareAnyLayer:
            case CompareClearLayer:
            case CompareOverlayLayer:
            default:
            {
              new_images=CompareImagesLayers(_images,(LayerMethod) parse,
                   _exception);
              break;
            }
            case MergeLayer:
            case FlattenLayer:
            case MosaicLayer:
            case TrimBoundsLayer:
            {
              new_images=MergeImageLayers(_images,(LayerMethod) parse,
                _exception);
              break;
            }
            case DisposeLayer:
            {
              new_images=DisposeImages(_images,_exception);
              break;
            }
            case OptimizeImageLayer:
            {
              new_images=OptimizeImageLayers(_images,_exception);
              break;
            }
            case OptimizePlusLayer:
            {
              new_images=OptimizePlusImageLayers(_images,_exception);
              break;
            }
            case OptimizeTransLayer:
            {
              OptimizeImageTransparency(_images,_exception);
              break;
            }
            case RemoveDupsLayer:
            {
              RemoveDuplicateLayers(&_images,_exception);
              break;
            }
            case RemoveZeroLayer:
            {
              RemoveZeroDelayLayers(&_images,_exception);
              break;
            }
            case OptimizeLayer:
            { /* General Purpose, GIF Animation Optimizer.  */
              new_images=CoalesceImages(_images,_exception);
              if (new_images == (Image *) NULL)
                break;
              _images=DestroyImageList(_images);
              _images=OptimizeImageLayers(new_images,_exception);
              if (_images == (Image *) NULL)
                break;
              new_images=DestroyImageList(new_images);
              OptimizeImageTransparency(_images,_exception);
              (void) RemapImages(_quantize_info,_images,(Image *) NULL,
                _exception);
              break;
            }
            case CompositeLayer:
            {
              Image
                *source;

              RectangleInfo
                geometry;

              CompositeOperator
                compose;

              const char*
                value;

              value=GetImageOption(_image_info,"compose");
              compose=OverCompositeOp;  /* Default to Over */
              if (value != (const char *) NULL)
                compose=(CompositeOperator) ParseCommandOption(
                      MagickComposeOptions,MagickFalse,value);

              /* Split image sequence at the first 'NULL:' image. */
              source=_images;
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
                    { /* Separate the two lists, junk the null: image.  */
                      source=SplitImageList(source->previous);
                      DeleteImageFromList(&source);
                    }
                }
              if (source == (Image *) NULL)
                {
                  (void) ThrowMagickException(_exception,GetMagickModule(),
                    OptionError,"MissingNullSeparator","layers Composite");
                  break;
                }
              /* Adjust offset with gravity and virtual canvas.  */
              SetGeometry(_images,&geometry);
              (void) ParseAbsoluteGeometry(_images->geometry,&geometry);
              geometry.width=source->page.width != 0 ?
                source->page.width : source->columns;
              geometry.height=source->page.height != 0 ?
               source->page.height : source->rows;
              GravityAdjustGeometry(_images->page.width != 0 ?
                _images->page.width : _images->columns,
                _images->page.height != 0 ? _images->page.height :
                _images->rows,_images->gravity,&geometry);

              /* Compose the two image sequences together */
              CompositeLayers(_images,compose,source,geometry.x,geometry.y,
                _exception);
              source=DestroyImageList(source);
              break;
            }
          }
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'm':
    {
      if (LocaleCompare("map",option+1) == 0)
        {
          CLIWandWarnReplaced("+remap");
          (void) RemapImages(_quantize_info,_images,(Image *) NULL,_exception);
          break;
        }
      if (LocaleCompare("metric",option+1) == 0)
        {
          (void) SetImageOption(_image_info,option+1,arg1);
          break;
        }
      if (LocaleCompare("morph",option+1) == 0)
        {
          Image
            *morph_image;

          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          morph_image=MorphImages(_images,StringToUnsignedLong(arg1),
            _exception);
          if (morph_image == (Image *) NULL)
            break;
          _images=DestroyImageList(_images);
          _images=morph_image;
          break;
        }
      if (LocaleCompare("mosaic",option+1) == 0)
        {
          /* REDIRECTED to use -layers mosaic instead */
          (void) CLIListOperatorImages(cli_wand,"-layers",option+1,NULL);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'p':
    {
      if (LocaleCompare("poly",option+1) == 0)
        {
          double
            *args;

          ssize_t
            count;

          /* convert argument string into an array of doubles */
          args = StringToArrayOfDoubles(arg1,&count,_exception);
          if (args == (double *) NULL )
            CLIWandExceptArgBreak(OptionError,"InvalidNumberList",option,arg1);
          new_images=PolynomialImage(_images,(size_t) (count >> 1),args,
           _exception);
          args=(double *) RelinquishMagickMemory(args);
          break;
        }
      if (LocaleCompare("process",option+1) == 0)
        {
          /* FUTURE: better parsing using ScriptToken() from string ??? */
          char
            **arguments;

          int
            j,
            number_arguments;

          arguments=StringToArgv(arg1,&number_arguments);
          if (arguments == (char **) NULL)
            break;
          if (strchr(arguments[1],'=') != (char *) NULL)
            {
              char
                breaker,
                quote,
                *token;

              const char
                *arguments;

              int
                next,
                status;

              size_t
                length;

              TokenInfo
                *token_info;

              /*
                Support old style syntax, filter="-option arg1".
              */
              assert(arg1 != (const char *) NULL);
              length=strlen(arg1);
              token=(char *) NULL;
              if (~length >= (MagickPathExtent-1))
                token=(char *) AcquireQuantumMemory(length+MagickPathExtent,
                  sizeof(*token));
              if (token == (char *) NULL)
                break;
              next=0;
              arguments=arg1;
              token_info=AcquireTokenInfo();
              status=Tokenizer(token_info,0,token,length,arguments,"","=",
                "\"",'\0',&breaker,&next,&quote);
              token_info=DestroyTokenInfo(token_info);
              if (status == 0)
                {
                  const char
                    *argv;

                  argv=(&(arguments[next]));
                  (void) InvokeDynamicImageFilter(token,&_images,1,&argv,
                    _exception);
                }
              token=DestroyString(token);
              break;
            }
          (void) SubstituteString(&arguments[1],"-","");
          (void) InvokeDynamicImageFilter(arguments[1],&_images,
            number_arguments-2,(const char **) arguments+2,_exception);
          for (j=0; j < number_arguments; j++)
            arguments[j]=DestroyString(arguments[j]);
          arguments=(char **) RelinquishMagickMemory(arguments);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 'r':
    {
      if (LocaleCompare("remap",option+1) == 0)
        {
          (void) RemapImages(_quantize_info,_images,(Image *) NULL,_exception);
          break;
        }
      if (LocaleCompare("reverse",option+1) == 0)
        {
          ReverseImageList(&_images);
          break;
        }
      CLIWandExceptionBreak(OptionError,"UnrecognizedOption",option);
    }
    case 's':
    {
      if (LocaleCompare("smush",option+1) == 0)
        {
          /* FUTURE: this option needs more work to make better */
          ssize_t
            offset;

          if (IsGeometry(arg1) == MagickFalse)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          offset=(ssize_t) StringToLong(arg1);
          new_images=SmushImages(_images,IsNormalOp,offset,_exception);
          break;
        }
      if (LocaleCompare("subimage",option+1) == 0)
        {
          Image
            *base_image,
            *compare_image;

          const char
            *value;

          MetricType
            metric;

          double
            similarity;

          RectangleInfo
            offset;

          base_image=GetImageFromList(_images,0);
          compare_image=GetImageFromList(_images,1);

          /* Comparision Metric */
          metric=UndefinedErrorMetric;
          value=GetImageOption(_image_info,"metric");
          if (value != (const char *) NULL)
            metric=(MetricType) ParseCommandOption(MagickMetricOptions,
              MagickFalse,value);

          new_images=SimilarityImage(base_image,compare_image,metric,0.0,
            &offset,&similarity,_exception);

          if (new_images != (Image *) NULL)
            {
              char
                result[MagickPathExtent];

              (void) FormatLocaleString(result,MagickPathExtent,"%lf",
                similarity);
              (void) SetImageProperty(new_images,"subimage:similarity",result,
                _exception);
              (void) FormatLocaleString(result,MagickPathExtent,"%+ld",(long)
                offset.x);
              (void) SetImageProperty(new_images,"subimage:x",result,
                _exception);
              (void) FormatLocaleString(result,MagickPathExtent,"%+ld",(long)
                offset.y);
              (void) SetImageProperty(new_images,"subimage:y",result,
                _exception);
              (void) FormatLocaleString(result,MagickPathExtent,
                "%lux%lu%+ld%+ld",(unsigned long) offset.width,(unsigned long)
                offset.height,(long) offset.x,(long) offset.y);
              (void) SetImageProperty(new_images,"subimage:offset",result,
                _exception);
            }
          break;
        }
      if (LocaleCompare("swap",option+1) == 0)
        {
        Image
          *p,
          *q,
          *swap;

        ssize_t
          index,
          swap_index;

        index=(-1);
        swap_index=(-2);
        if (IfNormalOp) {
          GeometryInfo
            geometry_info;

          MagickStatusType
            flags;

          swap_index=(-1);
          flags=ParseGeometry(arg1,&geometry_info);
          if ((flags & RhoValue) == 0)
            CLIWandExceptArgBreak(OptionError,"InvalidArgument",option,arg1);
          index=(ssize_t) geometry_info.rho;
          if ((flags & SigmaValue) != 0)
            swap_index=(ssize_t) geometry_info.sigma;
        }
        p=GetImageFromList(_images,index);
        q=GetImageFromList(_images,swap_index);
        if ((p == (Image *) NULL) || (q == (Image *) NULL)) {
          if (IfNormalOp)
            CLIWandExceptArgBreak(OptionError,"InvalidImageIndex",option,arg1)
          else
            CLIWandExceptionBreak(OptionError,"TwoOrMoreImagesRequired",option);
        }
        if (p == q)
          CLIWandExceptArgBreak(OptionError,"InvalidImageIndex",option,arg1);
        swap=CloneImage(p,0,0,MagickTrue,_exception);
        if (swap == (Image *) NULL)
          CLIWandExceptArgBreak(ResourceLimitError,"MemoryAllocationFailed",
            option,GetExceptionMessage(errno));
        ReplaceImageInList(&p,CloneImage(q,0,0,MagickTrue,_exception));
        ReplaceImageInList(&q,swap);
        _images=GetFirstImageInList(q);
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

  /* if new image list generated, replace existing image list */
  if (new_images == (Image *) NULL)
    return(status == 0 ? MagickFalse : MagickTrue);
  _images=DestroyImageList(_images);
  _images=GetFirstImageInList(new_images);
  return(status == 0 ? MagickFalse : MagickTrue);

#undef _image_info
#undef _images
#undef _exception
#undef _draw_info
#undef _quantize_info
#undef IfNormalOp
#undef IfPlusOp
#undef IsNormalOp
}