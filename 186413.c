static MagickBooleanType SetMSLAttributes(MSLInfo *msl_info,const char *keyword,
  const char *value)
{
  Image
    *attributes;

  DrawInfo
    *draw_info;

  ExceptionInfo
    *exception;

  GeometryInfo
    geometry_info;

  Image
    *image;

  ImageInfo
    *image_info;

  int
    flags;

  ssize_t
    n;

  assert(msl_info != (MSLInfo *) NULL);
  if (keyword == (const char *) NULL)
    return(MagickTrue);
  if (value == (const char *) NULL)
    return(MagickTrue);
  exception=msl_info->exception;
  n=msl_info->n;
  attributes=msl_info->attributes[n];
  image_info=msl_info->image_info[n];
  draw_info=msl_info->draw_info[n];
  image=msl_info->image[n];
  switch (*keyword)
  {
    case 'A':
    case 'a':
    {
      if (LocaleCompare(keyword,"adjoin") == 0)
        {
          ssize_t
            adjoin;

          adjoin=ParseCommandOption(MagickBooleanOptions,MagickFalse,value);
          if (adjoin < 0)
            ThrowMSLException(OptionError,"UnrecognizedType",value);
          image_info->adjoin=(MagickBooleanType) adjoin;
          break;
        }
      if (LocaleCompare(keyword,"alpha") == 0)
        {
          ssize_t
            alpha;

          alpha=ParseCommandOption(MagickAlphaOptions,MagickFalse,value);
          if (alpha < 0)
            ThrowMSLException(OptionError,"UnrecognizedType",value);
          if (image != (Image *) NULL)
            (void) SetImageAlphaChannel(image,(AlphaChannelType) alpha);
          break;
        }
      if (LocaleCompare(keyword,"antialias") == 0)
        {
          ssize_t
            antialias;

          antialias=ParseCommandOption(MagickBooleanOptions,MagickFalse,value);
          if (antialias < 0)
            ThrowMSLException(OptionError,"UnrecognizedGravityType",value);
          image_info->antialias=(MagickBooleanType) antialias;
          break;
        }
      if (LocaleCompare(keyword,"area-limit") == 0)
        {
          MagickSizeType
            limit;

          limit=MagickResourceInfinity;
          if (LocaleCompare(value,"unlimited") != 0)
            limit=(MagickSizeType) StringToDoubleInterval(value,100.0);
          (void) SetMagickResourceLimit(AreaResource,limit);
          break;
        }
      if (LocaleCompare(keyword,"attenuate") == 0)
        {
          (void) SetImageOption(image_info,keyword,value);
          break;
        }
      if (LocaleCompare(keyword,"authenticate") == 0)
        {
          (void) CloneString(&image_info->density,value);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'B':
    case 'b':
    {
      if (LocaleCompare(keyword,"background") == 0)
        {
          (void) QueryColorDatabase(value,&image_info->background_color,
            exception);
          break;
        }
      if (LocaleCompare(keyword,"bias") == 0)
        {
          if (image == (Image *) NULL)
            break;
          image->bias=StringToDoubleInterval(value,(double) QuantumRange+1.0);
          break;
        }
      if (LocaleCompare(keyword,"blue-primary") == 0)
        {
          if (image == (Image *) NULL)
            break;
          flags=ParseGeometry(value,&geometry_info);
          image->chromaticity.blue_primary.x=geometry_info.rho;
          image->chromaticity.blue_primary.y=geometry_info.sigma;
          if ((flags & SigmaValue) == 0)
            image->chromaticity.blue_primary.y=
              image->chromaticity.blue_primary.x;
          break;
        }
      if (LocaleCompare(keyword,"bordercolor") == 0)
        {
          (void) QueryColorDatabase(value,&image_info->border_color,
            exception);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare(keyword,"density") == 0)
        {
          (void) CloneString(&image_info->density,value);
          (void) CloneString(&draw_info->density,value);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'F':
    case 'f':
    {
      if (LocaleCompare(keyword,"fill") == 0)
        {
          (void) QueryColorDatabase(value,&draw_info->fill,exception);
          (void) SetImageOption(image_info,keyword,value);
          break;
        }
      if (LocaleCompare(keyword,"filename") == 0)
        {
          (void) CopyMagickString(image_info->filename,value,MaxTextExtent);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare(keyword,"gravity") == 0)
        {
          ssize_t
            gravity;

          gravity=ParseCommandOption(MagickGravityOptions,MagickFalse,value);
          if (gravity < 0)
            ThrowMSLException(OptionError,"UnrecognizedGravityType",value);
          (void) SetImageOption(image_info,keyword,value);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare(keyword,"id") == 0)
        {
          (void) SetImageProperty(attributes,keyword,value);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'M':
    case 'm':
    {
      if (LocaleCompare(keyword,"magick") == 0)
        {
          (void) CopyMagickString(image_info->magick,value,MaxTextExtent);
          break;
        }
      if (LocaleCompare(keyword,"mattecolor") == 0)
        {
          (void) QueryColorDatabase(value,&image_info->matte_color,
            exception);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare(keyword,"pointsize") == 0)
        {
          image_info->pointsize=StringToDouble(value,(char **) NULL);
          draw_info->pointsize=StringToDouble(value,(char **) NULL);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    case 'Q':
    case 'q':
    {
      if (LocaleCompare(keyword,"quality") == 0)
        {
          image_info->quality=StringToLong(value);
          if (image == (Image *) NULL)
            break;
          image->quality=StringToLong(value);
          break;
        }
      break;
    }
    case 'S':
    case 's':
    {
      if (LocaleCompare(keyword,"size") == 0)
        {
          (void) CloneString(&image_info->size,value);
          break;
        }
      if (LocaleCompare(keyword,"stroke") == 0)
        {
          (void) QueryColorDatabase(value,&draw_info->stroke,exception);
          (void) SetImageOption(image_info,keyword,value);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
    default:
    {
      ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
      break;
    }
  }
  return(MagickTrue);
}