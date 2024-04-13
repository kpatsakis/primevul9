static MagickBooleanType ImportFloatPixel(Image *image,const RectangleInfo *roi,
  const char *magick_restrict map,const QuantumType *quantum_map,
  const void *pixels,ExceptionInfo *exception)
{
  register const float
    *magick_restrict p;

  register Quantum
    *magick_restrict q;

  register ssize_t
    x;

  size_t
    length;

  ssize_t
    y;

  p=(const float *) pixels;
  if (LocaleCompare(map,"BGR") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"BGRA") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelAlpha(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"BGRP") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"I") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelGray(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"RGB") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"RGBA") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelAlpha(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  if (LocaleCompare(map,"RGBP") == 0)
    {
      for (y=0; y < (ssize_t) roi->height; y++)
      {
        q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) roi->width; x++)
        {
          SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
          p++;
          q+=GetPixelChannels(image);
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
    }
  length=strlen(map);
  for (y=0; y < (ssize_t) roi->height; y++)
  {
    q=GetAuthenticPixels(image,roi->x,roi->y+y,roi->width,1,exception);
    if (q == (Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) roi->width; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) length; i++)
      {
        switch (quantum_map[i])
        {
          case RedQuantum:
          case CyanQuantum:
          {
            SetPixelRed(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          case GreenQuantum:
          case MagentaQuantum:
          {
            SetPixelGreen(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          case BlueQuantum:
          case YellowQuantum:
          {
            SetPixelBlue(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          case AlphaQuantum:
          {
            SetPixelAlpha(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          case OpacityQuantum:
          {
            SetPixelAlpha(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          case BlackQuantum:
          {
            SetPixelBlack(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          case IndexQuantum:
          {
            SetPixelGray(image,ClampToQuantum(QuantumRange*(*p)),q);
            break;
          }
          default:
            break;
        }
        p++;
      }
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
  return(y < (ssize_t) roi->height ? MagickFalse : MagickTrue);
}