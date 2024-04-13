static MagickBooleanType ExportCharPixel(const Image *image,
  const RectangleInfo *roi,const char *magick_restrict map,
  const QuantumType *quantum_map,void *pixels,ExceptionInfo *exception)
{
  register const Quantum
    *magick_restrict p;

  register ssize_t
    x;

  register unsigned char
    *magick_restrict q;

  size_t
    length;

  ssize_t
    y;

  q=(unsigned char *) pixels;
  if (LocaleCompare(map,"BGR") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
          *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
          *q++=ScaleQuantumToChar(GetPixelRed(image,p));
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"BGRA") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
          *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
          *q++=ScaleQuantumToChar(GetPixelRed(image,p));
          *q++=ScaleQuantumToChar(GetPixelAlpha(image,p));
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"BGRP") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
          *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
          *q++=ScaleQuantumToChar(GetPixelRed(image,p));
          *q++=ScaleQuantumToChar((Quantum) 0);
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"I") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(ClampToQuantum(GetPixelIntensity(image,p)));
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"RGB") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(GetPixelRed(image,p));
          *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
          *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"RGBA") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(GetPixelRed(image,p));
          *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
          *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
          *q++=ScaleQuantumToChar(GetPixelAlpha(image,p));
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"RGBP") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToChar(GetPixelRed(image,p));
          *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
          *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
          *q++=ScaleQuantumToChar((Quantum) 0);
          p+=GetPixelChannels(image);
        }
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  length=strlen(map);
  for (y=0; y < (ssize_t) roi->height; y++)
  {
    p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) roi->width; x++)
    {
      register ssize_t
        i;

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
            *q=ScaleQuantumToChar(GetPixelAlpha(image,p));
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
  }
  return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
}