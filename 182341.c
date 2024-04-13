static void ExportCMYKOQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const PixelPacket *magick_restrict p,
  const IndexPacket *magick_restrict indexes,unsigned char *magick_restrict q,
  ExceptionInfo *exception)
{
  QuantumAny
    range;

  ssize_t
    x;

  if (image->colorspace != CMYKColorspace)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
        "ColorSeparatedImageRequired","`%s'",image->filename);
      return;
    }
  switch (quantum_info->depth)
  {
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToChar(GetPixelRed(p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelGreen(p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelBlue(p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelIndex(indexes+x));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelOpacity(p));
        q=PopCharPixel(pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 16:
    {
      unsigned short
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelRed(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelGreen(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelBlue(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelIndex(indexes+x));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelOpacity(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(GetPixelRed(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelGreen(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelBlue(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelIndex(indexes+x));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelOpacity(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 32:
    {
      unsigned int
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            float
              pixel;

            q=PopFloatPixel(quantum_info,(float) GetPixelRed(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelGreen(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelBlue(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelIndex(indexes+x),q);
            pixel=(float) (GetPixelOpacity(p));
            q=PopFloatPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(GetPixelRed(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelGreen(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelBlue(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelIndex(indexes+x));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelOpacity(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 64:
    {
      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          double
            pixel;

          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopDoublePixel(quantum_info,(double) GetPixelRed(p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelGreen(p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelBlue(p),q);
            q=PopDoublePixel(quantum_info,(double)
              GetPixelIndex(indexes+x),q);
            pixel=(double) (GetPixelOpacity(p));
            q=PopDoublePixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelRed(p),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelGreen(p),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelBlue(p),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelIndex(indexes+x),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelOpacity(p),range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}