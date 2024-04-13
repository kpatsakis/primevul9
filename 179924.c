static MagickBooleanType ExportLongPixel(const Image *image,
  const RectangleInfo *roi,const char *magick_restrict map,
  const QuantumType *quantum_map,void *pixels,ExceptionInfo *exception)
{
  register const Quantum
    *magick_restrict p;

  register ssize_t
    x;

  register unsigned int
    *magick_restrict q;

  size_t
    length;

  ssize_t
    y;

  q=(unsigned int *) pixels;
  if (LocaleCompare(map,"BGR") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        p=GetVirtualPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (p == (const Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
          *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
          *q++=ScaleQuantumToLong(GetPixelRed(image,p));
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
          *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
          *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
          *q++=ScaleQuantumToLong(GetPixelRed(image,p));
          *q++=ScaleQuantumToLong(GetPixelAlpha(image,p));
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
          *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
          *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
          *q++=ScaleQuantumToLong(GetPixelRed(image,p));
          *q++=0;
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
          *q++=ScaleQuantumToLong(ClampToQuantum(GetPixelIntensity(image,p)));
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
          *q++=ScaleQuantumToLong(GetPixelRed(image,p));
          *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
          *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
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
          *q++=ScaleQuantumToLong(GetPixelRed(image,p));
          *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
          *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
          *q++=ScaleQuantumToLong(GetPixelAlpha(image,p));
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
          *q++=ScaleQuantumToLong(GetPixelRed(image,p));
          *q++=ScaleQuantumToLong(GetPixelGreen(image,p));
          *q++=ScaleQuantumToLong(GetPixelBlue(image,p));
          *q++=0;
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
            *q=ScaleQuantumToLong(GetPixelAlpha(image,p));
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
  }
  return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
}