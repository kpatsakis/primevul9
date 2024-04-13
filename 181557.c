static MagickBooleanType StreamImagePixels(const StreamInfo *stream_info,
  const Image *image,ExceptionInfo *exception)
{
  QuantumInfo
    *quantum_info;

  QuantumType
    *quantum_map;

  register const Quantum
    *p;

  register ssize_t
    i,
    x;

  size_t
    length;

  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  length=strlen(stream_info->map);
  quantum_map=(QuantumType *) AcquireQuantumMemory(length,sizeof(*quantum_map));
  if (quantum_map == (QuantumType *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(MagickFalse);
    }
  for (i=0; i < (ssize_t) length; i++)
  {
    switch (stream_info->map[i])
    {
      case 'A':
      case 'a':
      {
        quantum_map[i]=AlphaQuantum;
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
        if (image->colorspace == CMYKColorspace)
          break;
        quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
        (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
          "ColorSeparatedImageRequired","`%s'",stream_info->map);
        return(MagickFalse);
      }
      case 'g':
      case 'G':
      {
        quantum_map[i]=GreenQuantum;
        break;
      }
      case 'I':
      case 'i':
      {
        quantum_map[i]=IndexQuantum;
        break;
      }
      case 'K':
      case 'k':
      {
        quantum_map[i]=BlackQuantum;
        if (image->colorspace == CMYKColorspace)
          break;
        quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
        (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
          "ColorSeparatedImageRequired","`%s'",stream_info->map);
        return(MagickFalse);
      }
      case 'M':
      case 'm':
      {
        quantum_map[i]=MagentaQuantum;
        if (image->colorspace == CMYKColorspace)
          break;
        quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
        (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
          "ColorSeparatedImageRequired","`%s'",stream_info->map);
        return(MagickFalse);
      }
      case 'o':
      case 'O':
      {
        quantum_map[i]=OpacityQuantum;
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
        if (image->colorspace == CMYKColorspace)
          break;
        quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
        (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
          "ColorSeparatedImageRequired","`%s'",stream_info->map);
        return(MagickFalse);
      }
      default:
      {
        quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
          "UnrecognizedPixelMap","`%s'",stream_info->map);
        return(MagickFalse);
      }
    }
  }
  quantum_info=stream_info->quantum_info;
  switch (stream_info->storage_type)
  {
    case CharPixel:
    {
      register unsigned char
        *q;

      q=(unsigned char *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar(GetPixelAlpha(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar((Quantum) 0);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(ClampToQuantum(GetPixelIntensity(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            *q++=ScaleQuantumToChar((Quantum) (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            *q++=ScaleQuantumToChar((Quantum) 0);
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=ScaleQuantumToChar(GetPixelRed(image,p));
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=ScaleQuantumToChar(GetPixelGreen(image,p));
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=ScaleQuantumToChar(GetPixelBlue(image,p));
              break;
            }
            case AlphaQuantum:
            {
              *q=ScaleQuantumToChar((Quantum) (GetPixelAlpha(image,p)));
              break;
            }
            case OpacityQuantum:
            {
              *q=ScaleQuantumToChar(GetPixelAlpha(image,p));
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=ScaleQuantumToChar(GetPixelBlack(image,p));
              break;
            }
            case IndexQuantum:
            {
              *q=ScaleQuantumToChar(ClampToQuantum(GetPixelIntensity(image,p)));
              break;
            }
            default:
              break;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    case DoublePixel:
    {
      register double
        *q;

      q=(double *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelAlpha(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=0.0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelIntensity(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelAlpha(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(double) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(double) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=0.0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=(double) ((QuantumScale*GetPixelRed(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=(double) ((QuantumScale*GetPixelGreen(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=(double) ((QuantumScale*GetPixelBlue(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case AlphaQuantum:
            {
              *q=(double) ((QuantumScale*GetPixelAlpha(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case OpacityQuantum:
            {
              *q=(double) ((QuantumScale*GetPixelAlpha(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=(double) ((QuantumScale*GetPixelBlack(image,p))*
                  quantum_info->scale+quantum_info->minimum);
              break;
            }
            case IndexQuantum:
            {
              *q=(double) ((QuantumScale*GetPixelIntensity(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            default:
              *q=0;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    case FloatPixel:
    {
      register float
        *q;

      q=(float *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*(Quantum) (GetPixelAlpha(image,p)))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=0.0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelIntensity(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelAlpha(image,p))*
              quantum_info->scale+quantum_info->minimum);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=(float) ((QuantumScale*GetPixelRed(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelGreen(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=(float) ((QuantumScale*GetPixelBlue(image,p))*
              quantum_info->scale+quantum_info->minimum);
            *q++=0.0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=(float) ((QuantumScale*GetPixelRed(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=(float) ((QuantumScale*GetPixelGreen(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=(float) ((QuantumScale*GetPixelBlue(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case AlphaQuantum:
            {
              *q=(float) ((QuantumScale*GetPixelAlpha(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case OpacityQuantum:
            {
              *q=(float) ((QuantumScale*GetPixelAlpha(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=(float) ((QuantumScale*GetPixelBlack(image,p))*
                  quantum_info->scale+quantum_info->minimum);
              break;
            }
            case IndexQuantum:
            {
              *q=(float) ((QuantumScale*GetPixelIntensity(image,p))*
                quantum_info->scale+quantum_info->minimum);
              break;
            }
            default:
              *q=0;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    case LongPixel:
    {
      register unsigned int
        *q;

      q=(unsigned int *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLong(GetPixelRed(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLong((Quantum) (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLong(GetPixelRed(image,p));
            *q++=0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(ClampToQuantum(GetPixelIntensity(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLong((Quantum) (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
            *q++=0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=ScaleQuantumToLong(GetPixelRed(image,p));
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=ScaleQuantumToLong(GetPixelGreen(image,p));
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=ScaleQuantumToLong(GetPixelBlue(image,p));
              break;
            }
            case AlphaQuantum:
            {
              *q=ScaleQuantumToLong((Quantum) (GetPixelAlpha(image,p)));
              break;
            }
            case OpacityQuantum:
            {
              *q=ScaleQuantumToLong(GetPixelAlpha(image,p));
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=ScaleQuantumToLong(GetPixelBlack(image,p));
              break;
            }
            case IndexQuantum:
            {
              *q=ScaleQuantumToLong(ClampToQuantum(GetPixelIntensity(image,p)));
              break;
            }
            default:
              break;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    case LongLongPixel:
    {
      register MagickSizeType
        *q;

      q=(MagickSizeType *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelRed(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelAlpha(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelRed(image,p));
            *q++=0U;
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(ClampToQuantum(
              GetPixelIntensity(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelBlue(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelBlue(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelAlpha(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToLongLong(GetPixelRed(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelGreen(image,p));
            *q++=ScaleQuantumToLongLong(GetPixelBlue(image,p));
            *q++=0U;
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=ScaleQuantumToLongLong(GetPixelRed(image,p));
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=ScaleQuantumToLongLong(GetPixelGreen(image,p));
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=ScaleQuantumToLongLong(GetPixelBlue(image,p));
              break;
            }
            case AlphaQuantum:
            {
              *q=ScaleQuantumToLongLong(GetPixelAlpha(image,p));
              break;
            }
            case OpacityQuantum:
            {
              *q=ScaleQuantumToLongLong(GetPixelAlpha(image,p));
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=ScaleQuantumToLongLong(GetPixelBlack(image,p));
              break;
            }
            case IndexQuantum:
            {
              *q=ScaleQuantumToLongLong(ClampToQuantum(
                GetPixelIntensity(image,p)));
              break;
            }
            default:
              *q=0;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    case QuantumPixel:
    {
      register Quantum
        *q;

      q=(Quantum *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=GetPixelBlue(image,p);
            *q++=GetPixelGreen(image,p);
            *q++=GetPixelRed(image,p);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=GetPixelBlue(image,p);
            *q++=GetPixelGreen(image,p);
            *q++=GetPixelRed(image,p);
            *q++=GetPixelAlpha(image,p);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=GetPixelBlue(image,p);
            *q++=GetPixelGreen(image,p);
            *q++=GetPixelRed(image,p);
            *q++=0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ClampToQuantum(GetPixelIntensity(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=GetPixelRed(image,p);
            *q++=GetPixelGreen(image,p);
            *q++=GetPixelBlue(image,p);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=GetPixelRed(image,p);
            *q++=GetPixelGreen(image,p);
            *q++=GetPixelBlue(image,p);
            *q++=GetPixelAlpha(image,p);
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=GetPixelRed(image,p);
            *q++=GetPixelGreen(image,p);
            *q++=GetPixelBlue(image,p);
            *q++=0U;
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=(Quantum) 0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=GetPixelRed(image,p);
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=GetPixelGreen(image,p);
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=GetPixelBlue(image,p);
              break;
            }
            case AlphaQuantum:
            {
              *q=(Quantum) (GetPixelAlpha(image,p));
              break;
            }
            case OpacityQuantum:
            {
              *q=GetPixelAlpha(image,p);
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=GetPixelBlack(image,p);
              break;
            }
            case IndexQuantum:
            {
              *q=ClampToQuantum(GetPixelIntensity(image,p));
              break;
            }
            default:
              *q=0;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    case ShortPixel:
    {
      register unsigned short
        *q;

      q=(unsigned short *) stream_info->pixels;
      if (LocaleCompare(stream_info->map,"BGR") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(GetPixelBlue(image,p));
            *q++=ScaleQuantumToShort(GetPixelGreen(image,p));
            *q++=ScaleQuantumToShort(GetPixelRed(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(GetPixelBlue(image,p));
            *q++=ScaleQuantumToShort(GetPixelGreen(image,p));
            *q++=ScaleQuantumToShort(GetPixelRed(image,p));
            *q++=ScaleQuantumToShort((Quantum) (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"BGRP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
            if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(GetPixelBlue(image,p));
            *q++=ScaleQuantumToShort(GetPixelGreen(image,p));
            *q++=ScaleQuantumToShort(GetPixelRed(image,p));
            *q++=0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"I") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(ClampToQuantum(
              GetPixelIntensity(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGB") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(GetPixelRed(image,p));
            *q++=ScaleQuantumToShort(GetPixelGreen(image,p));
            *q++=ScaleQuantumToShort(GetPixelBlue(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBA") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(GetPixelRed(image,p));
            *q++=ScaleQuantumToShort(GetPixelGreen(image,p));
            *q++=ScaleQuantumToShort(GetPixelBlue(image,p));
            *q++=ScaleQuantumToShort((Quantum) (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
          }
          break;
        }
      if (LocaleCompare(stream_info->map,"RGBP") == 0)
        {
          p=GetAuthenticPixelQueue(image);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) GetImageExtent(image); x++)
          {
            *q++=ScaleQuantumToShort(GetPixelRed(image,p));
            *q++=ScaleQuantumToShort(GetPixelGreen(image,p));
            *q++=ScaleQuantumToShort(GetPixelBlue(image,p));
            *q++=0;
            p+=GetPixelChannels(image);
          }
          break;
        }
      p=GetAuthenticPixelQueue(image);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) GetImageExtent(image); x++)
      {
        for (i=0; i < (ssize_t) length; i++)
        {
          *q=0;
          switch (quantum_map[i])
          {
            case RedQuantum:
            case CyanQuantum:
            {
              *q=ScaleQuantumToShort(GetPixelRed(image,p));
              break;
            }
            case GreenQuantum:
            case MagentaQuantum:
            {
              *q=ScaleQuantumToShort(GetPixelGreen(image,p));
              break;
            }
            case BlueQuantum:
            case YellowQuantum:
            {
              *q=ScaleQuantumToShort(GetPixelBlue(image,p));
              break;
            }
            case AlphaQuantum:
            {
              *q=ScaleQuantumToShort(GetPixelAlpha(image,p));
              break;
            }
            case OpacityQuantum:
            {
              *q=ScaleQuantumToShort(GetPixelAlpha(image,p));
              break;
            }
            case BlackQuantum:
            {
              if (image->colorspace == CMYKColorspace)
                *q=ScaleQuantumToShort(GetPixelBlack(image,p));
              break;
            }
            case IndexQuantum:
            {
              *q=ScaleQuantumToShort(ClampToQuantum(
                GetPixelIntensity(image,p)));
              break;
            }
            default:
              break;
          }
          q++;
        }
        p+=GetPixelChannels(image);
      }
      break;
    }
    default:
    {
      quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "UnrecognizedPixelMap","`%s'",stream_info->map);
      break;
    }
  }
  quantum_map=(QuantumType *) RelinquishMagickMemory(quantum_map);
  return(MagickTrue);
}