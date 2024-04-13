MagickExport MagickBooleanType ImportImagePixels(Image *image,const ssize_t x,
  const ssize_t y,const size_t width,const size_t height,const char *map,
  const StorageType type,const void *pixels,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  QuantumType
    *quantum_map;

  RectangleInfo
    roi;

  register ssize_t
    i;

  size_t
    length;

  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  length=strlen(map);
  quantum_map=(QuantumType *) AcquireQuantumMemory(length,sizeof(*quantum_map));
  if (quantum_map == (QuantumType *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  for (i=0; i < (ssize_t) length; i++)
  {
    switch (map[i])
    {
      case 'a':
      case 'A':
      {
        quantum_map[i]=AlphaQuantum;
        image->alpha_trait=BlendPixelTrait;
        break;
      }
      case 'B':
      case 'b':
      {
        quantum_map[i]=BlueQuantum;
        break;
      }
      case 'C':
      case 'c':
      {
        quantum_map[i]=CyanQuantum;
        (void) SetImageColorspace(image,CMYKColorspace,exception);
        break;
      }
      case 'g':
      case 'G':
      {
        quantum_map[i]=GreenQuantum;
        break;
      }
      case 'K':
      case 'k':
      {
        quantum_map[i]=BlackQuantum;
        (void) SetImageColorspace(image,CMYKColorspace,exception);
        break;
      }
      case 'I':
      case 'i':
      {
        quantum_map[i]=IndexQuantum;
        (void) SetImageColorspace(image,GRAYColorspace,exception);
        break;
      }
      case 'm':
      case 'M':
      {
        quantum_map[i]=MagentaQuantum;
        (void) SetImageColorspace(image,CMYKColorspace,exception);
        break;
      }
      case 'O':
      case 'o':
      {
        quantum_map[i]=OpacityQuantum;
        image->alpha_trait=BlendPixelTrait;
        break;
      }
      case 'P':
      case 'p':
      {
        quantum_map[i]=UndefinedQuantum;
        break;
      }
      case 'R':
      case 'r':
      {
        quantum_map[i]=RedQuantum;
        break;
      }
      case 'Y':
      case 'y':
      {
        quantum_map[i]=YellowQuantum;
        (void) SetImageColorspace(image,CMYKColorspace,exception);
        break;
      }
      default:
      {
        quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
          "UnrecognizedPixelMap","`%s'",map);
        return(MagickFalse);
      }
    }
  }
  if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    return(MagickFalse);
  /*
    Transfer the pixels from the pixel data to the image.
  */
  roi.width=width;
  roi.height=height;
  roi.x=x;
  roi.y=y;
  switch (type)
  {
    case CharPixel:
    {
      status=ImportCharPixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    case DoublePixel:
    {
      status=ImportDoublePixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    case FloatPixel:
    {
      status=ImportFloatPixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    case LongPixel:
    {
      status=ImportLongPixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    case LongLongPixel:
    {
      status=ImportLongLongPixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    case QuantumPixel:
    {
      status=ImportQuantumPixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    case ShortPixel:
    {
      status=ImportShortPixel(image,&roi,map,quantum_map,pixels,exception);
      break;
    }
    default:
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "UnrecognizedStorageType","`%d'",type);
      status=MagickFalse;
    }
  }
  quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
  return(status);
}